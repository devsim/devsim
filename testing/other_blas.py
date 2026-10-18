import scipy_openblas32
import os
import platform

#
# To use this example, install the scipy_openblas32 package.
# Note that this example is fragile and will not work if the
# external project changes.
#


system = platform.system()
if system == "Darwin":
    suffix = "dylib"
elif system == "Windows":
    suffix = "dll"
elif system == "Linux":
    suffix = "so"

os.environ["DEVSIM_MATH_LIBS"] = os.path.join(
    scipy_openblas32.get_lib_dir(), f"libscipy_openblas.{suffix}"
)
os.environ["DEVSIM_BLAS_PREFIX"] = "scipy_"
os.environ["DEVSIM_BLAS_SUFFIX"] = "_"

import devsim  # noqa: E402

print(devsim.get_parameter(name="info"))
