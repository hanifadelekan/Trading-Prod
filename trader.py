import os
import sys
import asyncio
import json
import logging
import hmac
import hashlib
import aiohttp
import numpy as np
from urllib.parse import urlencode
from dataclasses import dataclass
from typing import Dict, Optional, List
import websockets
import time
import uuid
import requests
# Configure logging first
# Configure logging first
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        # Set handler levels separately
        logging.FileHandler('market_maker.log'),  # Debug+ to file
        logging.StreamHandler()  # Info+ to console
    ]
)

# Set individual handler levels
logging.getLogger().handlers[0].setLevel(logging.DEBUG)  # File handler
logging.getLogger().handlers[1].setLevel(logging.INFO)   # Console handler
logger = logging.getLogger('MarketMaker')

# Environment variables for security
'''API_KEY = os.getenv("BINANCE_API_KEY")
SECRET_KEY = os.getenv("BINANCE_SECRET_KEY")
ENVIRONMENT = os.getenv("TRADING_ENV", "TESTNET")  # MAINNET or TESTNET
'''
SECRET_KEY = 'CLAPcXjcuvvGUsi7Zxz4iFlwbltj9qxvZMRxYxWtZUQaKnKtSHwyIANs5QkunRXO'
API_KEY = 'kTRUqoZp5aZyGhTVnnsp8SWRMWX618OvLuIYFiqswyioIlssQJgCduPjXa3J6GLt'
# Constants
WS_URL = "wss://stream.binance.com:9443/ws" 
REST_URL = "https://testnet.binance.vision" 

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

class OrderBook:
    def __init__(self):
        self.bids: Dict[float, float] = {}
        self.asks: Dict[float, float] = {}
        self.last_update_id: int = 0
        self.best_bid: float = 0.0
        self.best_ask: float = 0.0
        self.bids_dict = {}
        self.asks_dict = {}  
        


    def depthupdate(self, bids: List[List[str]], asks: List[List[str]]):
        for price, qty in bids:
            price = float(price)
            qty = float(qty)
            self.bids_dict[price] = qty

        for price, qty in asks:
            price = float(price)
            qty = float(qty)
            self.asks_dict[price] = qty
        
    def metric_calculation(self):
        bids = np.array(list(self.bids_dict.items()))
        asks = np.array(list(self.asks_dict.items()))
        if bids.any() and asks.any():
            #print(bids,asks)
            bids = bids[bids[:,1] != 0]
            asks = asks[asks[:,1] != 0]
            bids = bids[bids[:, 0].argsort()[::-1]]
            asks = asks[asks[:, 0].argsort()]
            self.best_bid = bids[0,0]
            self.best_ask = asks[0,0]
            maxlen = min(bids.shape[0],asks.shape[0])
            bids = bids[:maxlen,0] * bids[:maxlen,1]
            asks = asks[:maxlen,0] * asks[:maxlen,1]
            
            
            self.imbalances = bids/(bids + asks)
            alpha = 0.8

            # Generate weights using exponential decay
            weights = np.exp(-alpha * np.arange(maxlen))

            # Normalize the weights so they sum to 1
            weights /= np.sum(weights)

            # Calculate the weighted mean
            i = np.average(self.imbalances, weights=weights)
            midprice  = (self.best_ask + self.best_bid)/2
            wmidprice = (i*self.best_ask) + ((1-i)*self.best_bid)
            return wmidprice, self.best_bid, self.best_ask, midprice

    '''    @property
    def best_bid(self) -> Optional[float]:
        return max(self.bids.keys()) if self.bids else None

    @property
    def best_ask(self) -> Optional[float]:
        return min(self.asks.keys()) if self.asks else None

    @property
    def mid_price(self) -> Optional[float]:
        if self.best_bid and self.best_ask:
            return (self.best_bid + self.best_ask) / 2
        return None'''

class RiskManager:
    def __init__(self, symbol: str, max_long: float, max_short: float, max_order_size: float):
        self.symbol = symbol
        self.max_long = max_long
        self.max_short = max_short
        self.max_order_size = max_order_size
        self.position = 0.0  # Initialize position tracking

    def validate_order(self, order: Order) -> bool:
        potential_change = order.quantity if order.side == "BUY" else -order.quantity
        new_position = self.position + potential_change
        
        # Enforce both long and short limits
        if new_position > self.max_long:
            logger.warning(f"Long limit exceeded: {new_position}/{self.max_long}")
            return False
        if new_position < self.max_short:
            logger.warning(f"Short limit exceeded: {new_position}/{self.max_short}")
            return False
            
        if order.quantity > self.max_order_size:
            logger.warning(f"Order size exceeds limit: {order.quantity}")
            return False
            
        return True


class BinanceWebSocketProtocol(websockets.WebSocketClientProtocol):
    async def process_ping(self, data: bytes):
        """Immediately respond to Binance pings with matching pong"""
        await self.send_pong(data)
        logger.debug(f"Responded to Binance ping with payload: {data.hex()}")
class BinanceWebSocket:
    def __init__(self, symbol: str):
        self.symbol = symbol
        self.ws_url = WS_URL
        self.reconnect_delay = 5
        self.websocket = None
        self.running = False

    async def connect(self):
        while True:
            try:
                self.websocket = await websockets.connect(
                    self.ws_url,
                    create_protocol=BinanceWebSocketProtocol,  # Add this
                    ping_interval=None,  # Disable library pings
                    ping_timeout=20,  # Match Binance requirements
                    close_timeout=10
                )
           
                await self.subscribe()
                self.running = True
                logger.info(f"Connected to {self.symbol} WebSocket")
                #listener_task = asyncio.create_task(self.listen())
                '''ping_task = asyncio.create_task(self.keep_alive())
                await ping_task'''
                return
            except Exception as e:
                logger.error(f"Connection failed: {e}, retrying in {self.reconnect_delay}s")
                await asyncio.sleep(self.reconnect_delay)

    async def subscribe(self):
        subscribe_msg = {
            "method": "SUBSCRIBE",
            "params": [
                f"{self.symbol.lower()}@depth@100ms",
                f"{self.symbol.lower()}@trade"
            ],
            "id": 1
        }
        await self.websocket.send(json.dumps(subscribe_msg))

    async def receive(self):
        while self.running:
            try:
                message = await self.websocket.recv()
                yield json.loads(message)
            except websockets.ConnectionClosed:
                logger.warning("WebSocket connection closed, reconnecting...")
                await self.connect()
            except Exception as e:
                logger.error(f"Receive error: {e}")
                await asyncio.sleep(1)

    async def close(self):
        self.running = False
        if self.websocket:
            await self.websocket.close()
            logger.info("WebSocket closed")




class OrderManager:
    def __init__(self, symbol: str, max_long: float, max_short: float, max_order_size: float):
        self.symbol = symbol
        self.active_orders: Dict[str, Order] = {}
        self.order_book = OrderBook()
        self.risk_manager = RiskManager(
            symbol=symbol,
            max_long=max_long,
            max_short=max_short,
            max_order_size=max_order_size
        )
        self.order_timeout = 30
        self.lastUpdateId = 0
        self.count = 0
        self.active_bids = {}
        self.active_asks = {}
        self.signal = None
    def quotes(self,imbalance, bbid,bask,midprice):
        if imbalance > midprice:
            self.signal = 1
            bid =  bbid
            ask = bask + 0.01
        elif imbalance < midprice:
            self.signal = -1
            bid = bbid - 0.01
            ask = bask 
        elif imbalance == midprice:
            bid = bbid
            ask = bask
        return bid, ask

    async def process_trade(self, data: dict):
        """Update position with trade validation"""
        qty = float(data['q'])
        is_sell = data['m']  # Maker is seller
        
        new_position = self.risk_manager.position + (-qty if is_sell else qty)
        
        if (new_position > self.risk_manager.max_long or 
            new_position < self.risk_manager.max_short):
            logger.error("Position limit violation detected!")
            await self.cancel_all_orders()
            return
            
        self.risk_manager.position = new_position
        logger.info(f"Position updated: {new_position:.2f}")
    def trade_update(self,data):
        print(data)
        q = data['q']
        price = data['p']
        if data['m']:
            
            if price in self.order_book.bids_dict:
                self.order_book.bids_dict[price] -= q
        elif not data['m']:
            
            if price in self.order_book.asks_dict:
                self.order_book.asks_dict[price] -= q
   
            
    async def handle_market_data(self, data: dict):
        try:
            
            await self.process_update(data)
        
        except Exception as e:
            logger.error(f"Data handling error: {e}")
    async def manager(self,bid,ask):
        bid = round(bid,2)
        ask = round(ask,2)
        if bid not in self.active_bids:
            
            self.active_bids[bid] = 'pending'
            await self.place_order("BUY",bid , 0.5)
            
        if ask not in self.active_asks:
            
            self.active_asks[ask] = 'pending'
            await self.place_order("SELL", ask, 0.5)
            
        active_bids = self.active_bids.copy()
        for items in active_bids.items():
            if items[1][1] != self.signal and items[1] != 'pending':
                #print(items[1][1] , self.signal)
                await self.cancel_order(items[1][0],items[0],'BUY')
        active_asks = self.active_asks.copy()
        for items in active_asks.items():
            if items[1][1] != self.signal and items[1] != 'pending':
                #print(items[1][1] , self.signal)
                await self.cancel_order(items[1][0],items[0],'SELL')
        print(self.active_asks,self.active_bids,self.signal)
    async def process_update(self, data: dict):


            
        
    
        if data.get('e') == 'depthUpdate':
            U = data['U']
            u = data['u']
            if self.count == 0:
        
                while self.lastUpdateId < U:
                    time.sleep(1)
                    response = requests.get(f'https://api.binance.com/api/v3/depth?symbol={self.symbol}&limit=5000')
                    obook = json.loads(response.text)
                    self.lastUpdateId = obook['lastUpdateId']
                    self.order_book.bids_dict = {float(bid[0]): float(bid[1]) for bid in obook['bids']}
                    self.order_book.asks_dict = {float(ask[0]): float(ask[1]) for ask in obook['asks']}      
                    
                self.count += 1
            if u > self.lastUpdateId:
                self.order_book.depthupdate(data['b'], data['a'])
            
        elif data.get('e') == 'trade' and self.count != 0:
            #await self.process_trade(data)
            self.trade_update(data)
                
        if self.order_book.bids_dict:
            imbalance, bbid, bask,midprice = self.order_book.metric_calculation()
            bid, ask = self.quotes(imbalance,bbid, bask,midprice)
            await self.manager(bid,ask)
        #await self.rebalance_orders()


    async def cancel_stale_orders(self, current_time: float):
        """Cancel orders older than order_timeout seconds"""
        stale_ids = []
        for order_id, order in self.active_orders.items():
            if current_time - order.timestamp > self.order_timeout:
                stale_ids.append(order_id)
        
        for order_id in stale_ids:
            await self.cancel_order(order_id)
            logger.info(f"Cancelled stale order {order_id}")

    async def cancel_order(self, order_id: str,price,side):
        """Cancel a single order"""
    
        try:
            headers, params = self._sign_request({
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
                    if response.status == 200:
                        print(response.text)
                        #del self.active_orders[order_id]
                        logger.info(f"Order {order_id} cancelled")
                        if side == 'BUY':
                            #print(self.active_bids)
                            del self.active_bids[price]
                        elif side == 'SELL':
                            #print(self.active_asks)
                            del self.active_asks[price]
                        
        except Exception as e:
            logger.error(f"Cancel error: {e}")

    async def cancel_all_orders(self):
        """Cancel all active orders"""
        for order_id in list(self.active_orders.keys()):
            await self.cancel_order(order_id)
    # In OrderManager class, modify the rebalance_orders method
    async def rebalance_orders(self):
        current_time = time.time()
        await self.cancel_stale_orders(current_time)
        
        # Add null checks for best_bid and best_ask
        if self.order_book.best_bid is None or self.order_book.best_ask is None:
            logger.warning("Missing market data - skipping rebalance")
            return
        
        # Get position-aware parameters
        position = self.risk_manager.position
        max_long = self.risk_manager.max_long
        max_short = self.risk_manager.max_short
        
        # Calculate position-adjusted spread
        spread = self.order_book.best_ask - self.order_book.best_bid
        base_spread = spread * 0.1
        
        # Add safety checks for position ratios
        if max_long == 0 or max_short == 0:
            logger.error("Invalid position limits configured")
            return
        
        # Rest of the method remains the same...
        
        # Adjust quotes based on position
        if position > max_long * 0.8:  # 80% of long limit
            # Reduce buy orders
            buy_spread = base_spread * 2
            sell_spread = base_spread * 0.5
        elif position < max_short * 0.8:  # 80% of short limit
            # Reduce sell orders
            buy_spread = base_spread * 0.5
            sell_spread = base_spread * 2
        else:
            buy_spread = base_spread
            sell_spread = base_spread
            
        target_bid = self.order_book.best_bid + buy_spread
        target_ask = self.order_book.best_ask - sell_spread
        
        # Dynamic order sizing
        position_ratio = abs(position) / max(abs(max_long), abs(max_short))
        order_size = max(0.1, self.risk_manager.max_order_size * (1 - position_ratio))
        
        await self.place_order("BUY", target_bid, order_size)
        await self.place_order("SELL", target_ask, order_size)

    async def place_order(self, side: str, price: float, quantity: float):
        order = Order(
            order_id=str(uuid.uuid4()),
            symbol=self.symbol,
            side=side,
            price=price,
            quantity=0.6
        )
        
        # Validate against current position
        if self.risk_manager.validate_order(order):
            '''self.active_orders[order.order_id] = order'''
            pass
        await self.send_order(order)

    async def send_order(self, order: Order):
        try:
            # Get both headers and signed parameters
            headers, params = self._sign_request({  # Changed here
                "symbol": order.symbol,
                "side": order.side,
                "type": "LIMIT",
                "timeInForce": "GTC",
                "quantity": order.quantity,
                "price": order.price,
                "timestamp": int(time.time() * 1000)
            })
            
            async with aiohttp.ClientSession() as session:
                async with session.post(
                    f"{REST_URL}/api/v3/order",
                    headers=headers,
                    data=params  # Now using properly defined params
                ) as response:
                    result = await response.json()
                    if 'error' in result:
                        logger.error(f"Order failed: {result['error']}")
                    else:
                        print(result)
                        orderid = result['orderId']
                        side = result['side']
                        price = float(result['price'])
                        orderid = result['orderId']
                        if side == 'BUY':
                            p = round(price,2)
        
                            self.active_bids[p] = [orderid,self.signal]
              
                        elif side == 'SELL':
                            p = round(price,2)
                  
                            self.active_asks[p] = [orderid,self.signal]

                           
                        logger.info(f"Order {order.order_id} placed")

        except Exception as e:
            logger.error(f"Order placement error: {e}")
    def _sign_request(self, params: dict) -> tuple:
        """Signs request parameters according to Binance API requirements."""
        # Create working copy of parameters
        params_copy = params.copy()
        
        # Add required timestamp
        #params_copy['timestamp'] = int(time.time() * 1000)
        
        # Remove existing signature if present
        if 'signature' in params_copy:
            del params_copy['signature']
        
        # Sort parameters alphabetically by key
        #sorted_params = sorted(params_copy.items(), key=lambda x: x[0])
        
        # Create query string for signature generation
        #query = urlencode(sorted_params)
        query = '&'.join([f'{key}={value}' for key, value in params_copy.items()])
        # Generate HMAC SHA256 signature
        signature = hmac.new(
            SECRET_KEY.encode('utf-8'),
            query.encode('utf-8'),
            hashlib.sha256
        ).hexdigest()
        
        # Add signature to original parameters
        signed_params = {
            **params,  # Preserve original parameter order
            'signature': signature
        }
        
        # Create headers
        headers = {
            "X-MBX-APIKEY": API_KEY
        }
        
        return headers, signed_params

class MarketMaker:
    def __init__(self, symbol: str, max_long: float, max_short: float, max_order_size: float):
        self.symbol = symbol
        self.ws_client = BinanceWebSocket(symbol)
        self.order_manager = OrderManager(
            symbol=symbol,
            max_long=max_long,
            max_short=max_short,
            max_order_size=max_order_size
        )
        self.running = False
    async def listen(websocket):
        """Listen for messages and handle pongs."""
        async for message in websocket:
            data = json.loads(message)
            if "pong" in data:
                # Handle pong response (e.g., reset timeout counter)
                pass
            else:
                # Process other messages (trades, order book updates, etc.)
                #await handle_message(data)
                pass

    async def start(self):
        self.running = True
        await self.ws_client.connect()
        asyncio.create_task(self.handle_messages())
        await self.main_loop()
        

    async def main_loop(self):
        while self.running:
            try:
                # Add periodic health checks
                await asyncio.sleep(0)
            except Exception as e:
                logger.error(f"Main loop error: {e}")
            pass

    async def handle_messages(self):
        async for message in self.ws_client.receive():
            await self.order_manager.handle_market_data(message)

    async def stop(self):
        self.running = False
        await self.ws_client.close()
        await self.order_manager.cancel_all_orders()

if __name__ == "__main__":
    symbol = "AUCTIONUSDT"
    maker = MarketMaker(
        symbol=symbol,
        max_long=50,
        max_short=-50,
        max_order_size=10
    )

    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(maker.start())
    except KeyboardInterrupt:
        loop.run_until_complete(maker.stop())
    finally:
        loop.close()