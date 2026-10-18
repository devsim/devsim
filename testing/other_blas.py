import scipy_openblas32
import os
import platform

system = platform.system()
if system == 'Darwin':
  os.environ['DEVSIM_MATH_LIBS'] = os.path.join(scipy_openblas32.get_lib_dir(), 'libscipy_openblas.dylib')
  os.environ['DEVSIM_BLAS_PREFIX'] = 'scipy_'
  os.environ['DEVSIM_BLAS_SUFFIX'] = '_'
elif system == 'Windows':
  os.environ['DEVSIM_MATH_LIBS'] = os.path.join(scipy_openblas32.get_lib_dir(), 'libscipy_openblas.dll')
  os.environ['DEVSIM_BLAS_PREFIX'] = 'scipy_'
  os.environ['DEVSIM_BLAS_SUFFIX'] = '_'

import devsim
print(devsim.get_parameter(name='info'))
