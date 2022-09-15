import sys

if sys.version_info[0] != 3:
    raise ImportError(
        'module not available for Python %d.%d please contact technical support'
        % sys.version_info[:2]
    )

from .devsim_py3 import *
from .devsim_py3 import __version__




