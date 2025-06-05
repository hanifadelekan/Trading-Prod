import os

# Binance API Credentials
API_KEY = 'kTRUqoZp5aZyGhTVnnsp8SWRMWX618OvLuIYFiqswyioIlssQJgCduPjXa3J6GLt'
SECRET_KEY = 'CLAPcXjcuvvGUsi7Zxz4iFlwbltj9qxvZMRxYxWtZUQaKnKtSHwyIANs5QkunRXO'

# API Endpoints
WS_URL = "wss://stream.binance.com:9443/ws"
REST_URL = "https://testnet.binance.vision"

# Market Maker Parameters
SYMBOL = "AUCTIONUSDT"
MAX_LONG = 50.0
MAX_SHORT = -50.0
MAX_ORDER_SIZE = 10.0
ORDER_TIMEOUT_SECONDS = 30 # For stale orders

# Logging
LOG_FILE = 'market_maker.log'
LOG_LEVEL_FILE = 'DEBUG'
LOG_LEVEL_CONSOLE = 'INFO'