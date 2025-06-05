from dataclasses import dataclass

@dataclass
class Order:
    order_id: str
    symbol: str
    side: str  # BUY/SELL
    price: float
    quantity: float
    status: str = "PENDING"  # PENDING, FILLED, CANCELED, REJECTED
    filled_quantity: float = 0.0
    timestamp: float = 0.0