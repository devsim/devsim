
import sys
import os

os.environ['PATH'] = os.environ['PATH'] + os.pathsep + os.path.normpath(os.path.join(os.path.dirname(__file__), '../msys'))

if sys.version_info[0] == 3:
    from .symdiff_py3 import *
    from .symdiff_py3 import __version__
else:
    raise ImportError('module not available for Python %d.%d please contact technical support' % sys.version_info[0:2])


