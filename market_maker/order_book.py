import numpy as np
import logging

logger = logging.getLogger('MarketMaker.OrderBook')

class OrderBook:
    def __init__(self):
        self.bids_dict: dict = {}
        self.asks_dict: dict = {}
        self.last_update_id: int = 0
        self.best_bid: float = 0.0
        self.best_ask: float = 0.0

    def depth_update(self, bids: list[list[str]], asks: list[list[str]]):
        """Updates the order book with new bid/ask data."""
        for price, qty in bids:
            price = float(price)
            qty = float(qty)
            self.bids_dict[price] = qty

        for price, qty in asks:
            price = float(price)
            qty = float(qty)
            self.asks_dict[price] = qty
        
        # Remove zero quantity entries
        self.bids_dict = {p: q for p, q in self.bids_dict.items() if q > 0}
        self.asks_dict = {p: q for p, q in self.asks_dict.items() if q > 0}

    def calculate_metrics(self):
        """Calculates market metrics like weighted mid-price and best bid/ask."""
        if not self.bids_dict or not self.asks_dict:
            logger.debug("Order book empty, cannot calculate metrics.")
            return None, None, None, None

        bids = np.array(list(self.bids_dict.items()))
        asks = np.array(list(self.asks_dict.items()))

        # Filter out zero quantity entries (already done in depth_update, but good to be safe)
        bids = bids[bids[:,1] != 0]
        asks = asks[asks[:,1] != 0]

        if bids.size == 0 or asks.size == 0:
            logger.debug("No valid bids or asks after filtering zero quantities.")
            return None, None, None, None

        # Sort bids descending by price, asks ascending by price
        bids = bids[bids[:, 0].argsort()[::-1]]
        asks = asks[asks[:, 0].argsort()]
        
        self.best_bid = bids[0,0]
        self.best_ask = asks[0,0]

        maxlen = min(bids.shape[0], asks.shape[0])
        if maxlen == 0:
            logger.debug("Insufficient bids or asks to calculate weighted imbalance.")
            return None, self.best_bid, self.best_ask, (self.best_bid + self.best_ask) / 2

        bids_value = bids[:maxlen, 0] * bids[:maxlen, 1]
        asks_value = asks[:maxlen, 0] * asks[:maxlen, 1]
        
        # Avoid division by zero
        total_value = bids_value + asks_value
        imbalances = np.where(total_value != 0, bids_value / total_value, 0)
        
        alpha = 0.8
        weights = np.exp(-alpha * np.arange(maxlen))
        weights /= np.sum(weights)

        i = np.average(imbalances, weights=weights)
        midprice = (self.best_ask + self.best_bid) / 2
        wmidprice = (i * self.best_ask) + ((1 - i) * self.best_bid)
        
        return wmidprice, self.best_bid, self.best_ask, midprice