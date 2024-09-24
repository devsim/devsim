import wheel
import sys

# get_platform issue
# https://community.opalstack.com/d/1445-issue-installing-livekit/2
try:
    from wheel.bdist_wheel import get_platform
except ImportError:
    from wheel._bdist_wheel import get_platform

try:
    plat = get_platform('')
except TypeError:
    #TypeError: get_platform() takes 0 positional arguments but 1 was given
    plat = get_platform()
#
#print(plat)

prefix = None
for i in 'x86_64', 'arm64':
  p = plat.find(i)
  if p != -1:
      prefix = plat[0:p]
      break

if prefix is None:
  print('')
else:
  print(prefix+ sys.argv[1])
#
#
#
