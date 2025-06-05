import asyncio
import json
import logging
import time
import uuid
import aiohttp
import requests

from models import Order
from order_book import OrderBook
from risk_manager import RiskManager
from config import REST_URL, ORDER_TIMEOUT_SECONDS, SYMBOL
from utils import sign_request

logger = logging.getLogger('MarketMaker.OrderManager')

class OrderManager:
    def __init__(self, symbol: str, max_long: float, max_short: float, max_order_size: float):
        self.symbol = symbol
        self.active_orders: dict[str, Order] = {}
        self.order_book = OrderBook()
        self.risk_manager = RiskManager(
            symbol=symbol,
            max_long=max_long,
            max_short=max_short,
            max_order_size=max_order_size
        )
        self.order_timeout = ORDER_TIMEOUT_SECONDS
        self.last_update_id = 0
        self.initial_order_book_synced = False
        self.active_bids = {}  # {price: [order_id, signal]}
        self.active_asks = {}  # {price: [order_id, signal]}
        self.signal = None

    def _determine_quotes(self, imbalance: float, best_bid: float, best_ask: float, midprice: float):
        """Determines bid and ask prices based on imbalance."""
        if imbalance is None:
            return best_bid, best_ask # Fallback if imbalance calculation failed

        if imbalance > midprice:
            self.signal = 1
            bid = best_bid
            ask = best_ask + 0.01
        elif imbalance < midprice:
            self.signal = -1
            bid = best_bid - 0.01
            ask = best_ask
        elif imbalance == midprice:
            bid = best_bid
            ask = best_ask
        return round(bid, 2), round(ask, 2)

    async def _sync_initial_order_book(self):
        """Fetches the initial order book snapshot."""
        while not self.initial_order_book_synced:
            try:
                response = requests.get(f'{REST_URL}/api/v3/depth?symbol={self.symbol}&limit=1000')
                response.raise_for_status()  # Raise an exception for HTTP errors
                obook = response.json()
                
                self.order_book.bids_dict = {float(bid[0]): float(bid[1]) for bid in obook['bids']}
                self.order_book.asks_dict = {float(ask[0]): float(ask[1]) for ask in obook['asks']}
                self.order_book.last_update_id = obook['lastUpdateId']
                self.last_update_id = obook['lastUpdateId']
                self.initial_order_book_synced = True
                logger.info(f"Initial order book synced with lastUpdateId: {self.last_update_id}")
            except requests.exceptions.RequestException as e:
                logger.error(f"Failed to sync initial order book: {e}. Retrying in 5s.")
                await asyncio.sleep(5)
            except json.JSONDecodeError:
                logger.error("Failed to decode JSON from order book snapshot. Retrying in 5s.")
                await asyncio.sleep(5)


    async def handle_market_data(self, data: dict):
        """Processes incoming market data from the WebSocket."""
        if not self.initial_order_book_synced:
            await self._sync_initial_order_book()
            return

        event_type = data.get('e')
        
        if event_type == 'depthUpdate':
            U = data['U'] # first update ID in event
            u = data['u'] # final update ID in event

            if u < self.last_update_id:
                logger.debug(f"Skipping old depth update (u={u}) as current lastUpdateId is {self.last_update_id}")
                return
            
            if U > self.last_update_id + 1:
                logger.warning(f"Gap in depth update sequence detected! U={U}, lastUpdateId={self.last_update_id}. Resyncing order book.")
                self.initial_order_book_synced = False
                await self._sync_initial_order_book()
                return

            self.order_book.depth_update(data['b'], data['a'])
            self.order_book.last_update_id = u
            self.last_update_id = u
            logger.debug(f"Depth update processed. New lastUpdateId: {self.last_update_id}")

        elif event_type == 'trade':
            # This is where you would process trade data to update your position
            # Currently, the position update logic is simplified; a full implementation
            # would involve matching trades against your open orders.
            # For now, let's assume trades affect the order book's view of liquidity.
            self.risk_manager.update_position(float(data['q']), data['m']) # data['m'] is 'is_maker' which means trade was a sell when true

        # Only calculate metrics and manage orders if the order book is populated
        if self.order_book.bids_dict and self.order_book.asks_dict:
            wmidprice, bbid, bask, midprice = self.order_book.calculate_metrics()
            if wmidprice is not None:
                bid, ask = self._determine_quotes(wmidprice, bbid, bask, midprice)
                await self._manage_quotes(bid, ask)
        else:
            logger.debug("Order book not sufficiently populated for metric calculation.")
            
        await self._cancel_stale_orders(time.time())

    async def _manage_quotes(self, desired_bid: float, desired_ask: float):
        """Manages placement and cancellation of limit orders."""
        
        # Manage bids
        # Cancel bids that are no longer at the desired price or signal
        bids_to_cancel = []
        for price, order_info in self.active_bids.items():
            order_id, order_signal = order_info
            if price != desired_bid or order_signal != self.signal:
                bids_to_cancel.append((order_id, price, 'BUY'))
        
        for order_id, price, side in bids_to_cancel:
            await self._cancel_single_order(order_id, price, side)

        # Place new bid if none exists at the desired price/signal
        if desired_bid not in self.active_bids or self.active_bids[desired_bid][1] != self.signal:
            await self.place_order("BUY", desired_bid, 0.6) # Using hardcoded quantity for now

        # Manage asks
        # Cancel asks that are no longer at the desired price or signal
        asks_to_cancel = []
        for price, order_info in self.active_asks.items():
            order_id, order_signal = order_info
            if price != desired_ask or order_signal != self.signal:
                asks_to_cancel.append((order_id, price, 'SELL'))

        for order_id, price, side in asks_to_cancel:
            await self._cancel_single_order(order_id, price, side)

        # Place new ask if none exists at the desired price/signal
        if desired_ask not in self.active_asks or self.active_asks[desired_ask][1] != self.signal:
            await self.place_order("SELL", desired_ask, 0.6) # Using hardcoded quantity for now


    async def _cancel_stale_orders(self, current_time: float):
        """Cancel orders older than order_timeout seconds"""
        stale_ids = []
        for order_id, order in self.active_orders.items():
            if current_time - order.timestamp > self.order_timeout:
                stale_ids.append(order_id)
        
        for order_id in stale_ids:
            # Note: We need the price and side to remove from active_bids/asks
            # This indicates a potential need to store price/side in active_orders dict
            # or refactor how active_bids/asks are managed.
            # For now, this just removes from active_orders.
            await self._cancel_single_order(order_id, None, None) # Price and Side are placeholders if not in active_orders
            logger.info(f"Cancelled stale order {order_id}")


    async def _cancel_single_order(self, order_id: str, price: float, side: str):
        """Cancels a single order on Binance."""
        try:
            headers, params = sign_request({
                "symbol": self.symbol,
                "orderId": order_id,
                'timestamp': int(time.time() * 1000)
            })
            
            async with aiohttp.ClientSession() as session:
                async with session.delete(
                    f"{REST_URL}/api/v3/order",
                    headers=headers,
                    params=params
                ) as response:
                    response_text = await response.text()
                    if response.status == 200:
                        logger.info(f"Order {order_id} cancelled successfully.")
                        if order_id in self.active_orders:
                            del self.active_orders[order_id]
                        if side == 'BUY' and price in self.active_bids and self.active_bids[price][0] == order_id:
                            del self.active_bids[price]
                        elif side == 'SELL' and price in self.active_asks and self.active_asks[price][0] == order_id:
                            del self.active_asks[price]
                    else:
                        logger.error(f"Failed to cancel order {order_id}. Status: {response.status}, Response: {response_text}")
        except Exception as e:
            logger.error(f"Error cancelling order {order_id}: {e}")

    async def cancel_all_orders(self):
        """Cancels all active orders tracked by the manager."""
        logger.info("Attempting to cancel all active orders...")
        headers, params = sign_request({
            "symbol": self.symbol,
            "timestamp": int(time.time() * 1000)
        })

        try:
            async with aiohttp.ClientSession() as session:
                async with session.delete(
                    f"{REST_URL}/api/v3/openOrders",
                    headers=headers,
                    params=params
                ) as response:
                    response_text = await response.text()
                    if response.status == 200:
                        logger.info(f"All open orders cancelled successfully. Response: {response_text}")
                        self.active_orders.clear()
                        self.active_bids.clear()
                        self.active_asks.clear()
                    else:
                        logger.error(f"Failed to cancel all orders. Status: {response.status}, Response: {response_text}")
        except Exception as e:
            logger.error(f"Error during bulk order cancellation: {e}")


    async def place_order(self, side: str, price: float, quantity: float):
        """Places a new limit order on Binance."""
        order_id = str(uuid.uuid4())
        order = Order(
            order_id=order_id,
            symbol=self.symbol,
            side=side,
            price=price,
            quantity=quantity,
            timestamp=time.time()
        )
        
        if not self.risk_manager.validate_order(order):
            logger.warning(f"Order {order.order_id} not placed due to risk validation.")
            return

        headers, params = sign_request({
            "symbol": order.symbol,
            "side": order.side,
            "type": "LIMIT",
            "timeInForce": "GTC",
            "quantity": order.quantity,
            "price": order.price,
            "newClientOrderId": order.order_id, # Use your generated UUID here
            "timestamp": int(time.time() * 1000)
        })
        
        try:
            async with aiohttp.ClientSession() as session:
                async with session.post(
                    f"{REST_URL}/api/v3/order",
                    headers=headers,
                    data=params
                ) as response:
                    result = await response.json()
                    if response.status == 200:
                        binance_order_id = result.get('orderId')
                        if binance_order_id:
                            order.status = "PENDING" # Or "NEW" depending on Binance response
                            self.active_orders[order.order_id] = order # Store with your UUID
                            
                            # Update active_bids/asks with Binance's orderId
                            rounded_price = round(float(result['price']), 2)
                            if side == 'BUY':
                                self.active_bids[rounded_price] = [binance_order_id, self.signal]
                            elif side == 'SELL':
                                self.active_asks[rounded_price] = [binance_order_id, self.signal]
                            
                            logger.info(f"Order {order.order_id} ({binance_order_id}) placed successfully. Details: {result}")
                        else:
                            logger.error(f"Order placement response missing orderId: {result}")
                            order.status = "REJECTED"
                    else:
                        logger.error(f"Order placement failed: {result}. Status: {response.status}")
                        order.status = "REJECTED"

        except Exception as e:
            logger.error(f"Order placement error for {order.order_id}: {e}")
            order.status = "REJECTED"