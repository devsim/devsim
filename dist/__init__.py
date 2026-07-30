import sys
import os

#TODO:
#https://stackoverflow.com/questions/6677424/how-do-i-import-variable-packages-in-python-like-using-variable-variables-i
#imported = getattr(__import__(package, fromlist=[name]), name)

original_path = os.environ['PATH']

if (os.name == 'nt') and ('DEVSIM_MATH_LIBS' not in os.environ):
    libbin = os.path.join('Library', 'bin')
    if libbin not in os.environ['PATH']:
        os.environ['PATH'] += os.pathsep + os.path.join(sys.prefix, libbin)

if sys.version_info[0] == 3:
    from .devsim_py3 import *
    from .devsim_py3 import __version__
else:
    raise ImportError('module not available for Python %d.%d please contact technical support' % sys.version_info[0:2])

if original_path != os.environ['PATH']:
    os.environ['PATH'] = original_path

if get_parameter(name='direct_solver') == 'unknown':
  print("loading UMFPACK 5.1 as direct solver")
  from .umfpack import umfshim


