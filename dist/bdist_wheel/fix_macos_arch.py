import wheel
import wheel.bdist_wheel
import sys

try:
    plat = wheel.bdist_wheel.get_platform('')
except TypeError:
    #TypeError: get_platform() takes 0 positional arguments but 1 was given
    plat = wheel.bdist_wheel.get_platform()
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
