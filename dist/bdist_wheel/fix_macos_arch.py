import wheel
import wheel.bdist_wheel
import sys

plat = wheel.bdist_wheel.get_platform('')
#print(plat)

for i in 'x86_64', 'arm64':
  p = plat.find(i)
  if p != -1:
      prefix = plat[0:p]
      break

print(prefix+ sys.argv[1])
#
#
#
