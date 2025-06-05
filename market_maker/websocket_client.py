import asyncio
import json
import logging
import websockets
from websockets.client import WebSocketClientProtocol

from config import WS_URL

logger = logging.getLogger('MarketMaker.WebSocket')

class BinanceWebSocketProtocol(WebSocketClientProtocol):
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
                    create_protocol=BinanceWebSocketProtocol,
                    ping_interval=None,
                    ping_timeout=20,
                    close_timeout=10
                )
                await self.subscribe()
                self.running = True
                logger.info(f"Connected to {self.symbol} WebSocket")
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
        logger.info(f"Subscribed to {self.symbol} depth and trade streams.")

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