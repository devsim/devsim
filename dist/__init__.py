import sys

#TODO:
#https://stackoverflow.com/questions/6677424/how-do-i-import-variable-packages-in-python-like-using-variable-variables-i
#imported = getattr(__import__(package, fromlist=[name]), name)

if sys.version_info[0] == 3:
    from .devsim_py3 import *
    from .devsim_py3 import __version__
else:
    raise ImportError('module not available for Python %d.%d please contact technical support' % sys.version_info[0:2])

if get_parameter(name='direct_solver') == 'unknown':
  print("loading UMFPACK 5.1 as direct solver")
  from .umfpack import umfshim


