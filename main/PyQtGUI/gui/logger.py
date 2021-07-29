import logging
_logger = None

def log(message):
    if _logger is not None:
        try:
            _logger(str(message).strip())
        except Exception as e:
            logging.debug(e)

def set_logger(logger):
    global _logger
    _logger = logger

def setup_logging(logfile=None):
    _filemode = 'a'
    _format= '%(asctime)s,%(msecs)d %(name)s %(levelname)s %(message)s'
    _datefmt= '%H:%M:%S'
    _level = logging.DEBUG
    logging.basicConfig(filename=logfile, filemode=_filemode, format=_format, datefmt=_datefmt, level=_level)
    global _logger
    _logger = lambda message: logging.debug(message)
