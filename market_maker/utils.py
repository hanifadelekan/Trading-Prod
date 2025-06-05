import logging
import hmac
import hashlib
from urllib.parse import urlencode
import time
from config import API_KEY, SECRET_KEY

def setup_logging():
    """Configures logging for the application."""
    logging.basicConfig(
        level=logging.DEBUG, # Overall lowest level
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.FileHandler('market_maker.log'),
            logging.StreamHandler()
        ]
    )
    # Set individual handler levels
    logging.getLogger().handlers[0].setLevel(logging.DEBUG)  # File handler
    logging.getLogger().handlers[1].setLevel(logging.INFO)   # Console handler

def sign_request(params: dict) -> tuple:
    """Signs request parameters according to Binance API requirements."""
    params_copy = params.copy()
    
    if 'signature' in params_copy:
        del params_copy['signature']
    
    # Sort parameters alphabetically by key and then encode
    query = urlencode(sorted(params_copy.items()))
    
    signature = hmac.new(
        SECRET_KEY.encode('utf-8'),
        query.encode('utf-8'),
        hashlib.sha256
    ).hexdigest()
    
    signed_params = {
        **params,
        'signature': signature
    }
    
    headers = {
        "X-MBX-APIKEY": API_KEY
    }
    
    return headers, signed_params