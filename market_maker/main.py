import asyncio
import logging
from market_maker import MarketMaker
from config import SYMBOL, MAX_LONG, MAX_SHORT, MAX_ORDER_SIZE
from utils import setup_logging

if __name__ == "__main__":
    setup_logging()
    
    logger = logging.getLogger('MarketMakerApp')

    maker = MarketMaker(
        symbol=SYMBOL,
        max_long=MAX_LONG,
        max_short=MAX_SHORT,
        max_order_size=MAX_ORDER_SIZE
    )

    loop = asyncio.get_event_loop()
    try:
        logger.info(f"Starting Market Maker for {SYMBOL}...")
        loop.run_until_complete(maker.start())
    except KeyboardInterrupt:
        logger.info("KeyboardInterrupt detected. Stopping Market Maker...")
        loop.run_until_complete(maker.stop())
    finally:
        logger.info("Market Maker application finished.")
        loop.close()