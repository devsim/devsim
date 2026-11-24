# ruff: noqa: E402
import sys as _sys
import os as _os


def _fix_windows_path():
    original_path = None
    if (_os.name == "nt") and ("DEVSIM_MATH_LIBS" not in _os.environ):
        libbin = _os.path.join("Library", "bin")
        if libbin not in _os.environ["PATH"]:
            original_path = _os.environ["PATH"]
            _os.environ["PATH"] += _os.pathsep + _os.path.join(_sys.prefix, libbin)
    return original_path


_original_path = _fix_windows_path()

# Last resort when no BLAS/LAPACK is found: use scipy-openblas32 if it is
# installed.  The compiled module reads the environment while it is imported,
# so this has to be decided here.  See _math_libs.py.
from . import _math_libs

_blas_env = _math_libs.apply_scipy_openblas_fallback()

try:
    from .devsim_py3 import *  # noqa: F403
    from .devsim_py3 import __version__  # noqa: F401
    from .devsim_py3 import get_parameter
    from .devsim_py3 import reset_devsim
finally:
    _math_libs.restore_environment(_blas_env)

if _original_path is not None:
    _os.environ["PATH"] = _original_path

if get_parameter(name="direct_solver") == "unknown":
    print("loading UMFPACK 5.1 as direct solver")
    from .umfpack import umfshim as _umfshim

    _original_reset_devsim = reset_devsim

    def reset_devsim():
        _original_reset_devsim()
        _umfshim.reload_umfpack()
