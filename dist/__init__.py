import sys

#TODO:
#https://stackoverflow.com/questions/6677424/how-do-i-import-variable-packages-in-python-like-using-variable-variables-i
#imported = getattr(__import__(package, fromlist=[name]), name)

if sys.version_info[0] == 2:
  from devsim_py27 import *
  from devsim_py27 import __version__
elif sys.version_info[0] == 3 and sys.version_info[1] == 6:
  from .devsim_py36 import *
  from .devsim_py36 import __version__
elif sys.version_info[0] == 3 and sys.version_info[1] == 7:
  from .devsim_py37 import *
  from .devsim_py37 import __version__
else:
  raise ImportError('module not available for Python %d.%d please contact technical support' % sys.version_info[0:2])




