import logging
from models import Order

logger = logging.getLogger('MarketMaker.RiskManager')

class RiskManager:
    def __init__(self, symbol: str, max_long: float, max_short: float, max_order_size: float):
        self.symbol = symbol
        self.max_long = max_long
        self.max_short = max_short
        self.max_order_size = max_order_size
        self.position = 0.0  # Initialize position tracking

    def validate_order(self, order: Order) -> bool:
        """Checks if a potential order adheres to risk limits."""
        potential_change = order.quantity if order.side == "BUY" else -order.quantity
        new_position = self.position + potential_change
        
        if new_position > self.max_long:
            logger.warning(f"Order rejected: Long limit exceeded ({new_position}/{self.max_long}) for order {order.order_id}")
            return False
        if new_position < self.max_short:
            logger.warning(f"Order rejected: Short limit exceeded ({new_position}/{self.max_short}) for order {order.order_id}")
            return False
            
        if order.quantity > self.max_order_size:
            logger.warning(f"Order rejected: Order size exceeds limit ({order.quantity}/{self.max_order_size}) for order {order.order_id}")
            return False
            
        return True

    def update_position(self, quantity: float, is_sell: bool):
        """Updates the current position based on a trade."""
        change = -quantity if is_sell else quantity
        new_position = self.position + change
        
        if (new_position > self.max_long + 0.001 or  # Add a small buffer for floating point
            new_position < self.max_short - 0.001):
            logger.critical(f"Position limit violation detected after trade: {new_position:.2f}! Action required.")
            # In a real system, this would trigger emergency actions.
        
        self.position = new_position
        logger.info(f"Position updated: {self.position:.2f}")