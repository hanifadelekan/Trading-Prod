import asyncio
import logging

from websocket_client import BinanceWebSocket
from order_manager import OrderManager
from config import SYMBOL, MAX_LONG, MAX_SHORT, MAX_ORDER_SIZE

logger = logging.getLogger('MarketMaker.Core')

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

    async def start(self):
        """Starts the Market Maker application."""
        self.running = True
        logger.info(f"Market Maker for {self.symbol} starting...")
        
        # Connect WebSocket and start listening
        await self.ws_client.connect()
        asyncio.create_task(self._handle_websocket_messages())
        
        # Start the main loop (e.g., for periodic checks or rebalancing)
        await self._main_loop()
        

    async def _main_loop(self):
        """The main execution loop for the Market Maker."""
        while self.running:
            try:
                # This loop can be used for periodic tasks, e.g.,
                # await self.order_manager.rebalance_orders() # if you re-introduce this logic
                await asyncio.sleep(1) # Keep the event loop responsive
            except Exception as e:
                logger.error(f"Main loop error: {e}")
            
    async def _handle_websocket_messages(self):
        """Consumes messages from the WebSocket and passes them to the OrderManager."""
        async for message in self.ws_client.receive():
            await self.order_manager.handle_market_data(message)

    async def stop(self):
        """Stops the Market Maker application gracefully."""
        logger.info("Market Maker stopping...")
        self.running = False
        await self.ws_client.close()
        await self.order_manager.cancel_all_orders()
        logger.info("Market Maker stopped.")