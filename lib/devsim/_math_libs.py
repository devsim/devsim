# SPDX-License-Identifier: Apache-2.0
"""
Last-resort selection of scipy-openblas32 as the BLAS/LAPACK provider.

The compiled module loads its math libraries once, while it is imported, and
the free-threaded build cannot be imported a second time after a failure.  A
fallback therefore has to be decided *before* ``devsim_py3`` is imported.

The compiled module looks for the Intel MKL when ``DEVSIM_MATH_LIBS`` is not
set, and then for ``libopenblas``, ``liblapack`` and ``libblas``.  Setting
``DEVSIM_MATH_LIBS`` switches that search off, so the environment is only
changed when none of those can be found.  The search is repeated here with
``ctypes`` using the same names and the same load flags, so that a library
found here is the same handle the compiled module gets later.

Nothing in this module imports ``devsim_py3`` or ``scipy_openblas32`` at
import time.  Set ``DEVSIM_NO_SCIPY_OPENBLAS=1`` to disable the fallback.
"""

import ctypes as _ctypes
import importlib as _importlib
import os as _os
import platform as _platform
import re as _re

OPT_OUT_VARIABLE = "DEVSIM_NO_SCIPY_OPENBLAS"
BLAS_PREFIX = "scipy_"
BLAS_SUFFIX = "_"

# Symbols the compiled module requires (default naming, see BlasHeaders.cc).
_REQUIRED_SYMBOLS = (
    "dgetrf_",
    "zgetrf_",
    "dgetrs_",
    "zgetrs_",
    "drotg_",
    "zrotg_",
)

_MKL_MAX_VERSION_TESTED_FOR = 5


def shared_library_extension(system=None):
    """File extension of shared libraries on this platform, or None."""
    if system is None:
        system = _platform.system()
    return {"Windows": "dll", "Darwin": "dylib", "Linux": "so"}.get(system)


def _mkl_names(system):
    # Same names as LoadIntelMKL(); no MKL is searched for on macOS.
    if system == "Windows":
        return ["mkl_rt.dll"] + [
            "mkl_rt.%d.dll" % i for i in range(1, _MKL_MAX_VERSION_TESTED_FOR + 1)
        ]
    if system == "Linux":
        return ["libmkl_rt.so"] + [
            "libmkl_rt.so.%d" % i for i in range(1, _MKL_MAX_VERSION_TESTED_FOR + 1)
        ]
    return []


def _default_names(system):
    ext = shared_library_extension(system)
    if ext is None:
        return []
    return ["libopenblas.%s" % ext, "liblapack.%s" % ext, "libblas.%s" % ext]


def _open_library(name):
    """
    Open a shared library the way the compiled module does.

    Returns the ctypes handle or None.
    """
    if _os.name == "nt":
        # winmode=0 is the plain LoadLibrary search, which includes PATH.
        # The ctypes default does not search PATH.
        kwargs = {"winmode": 0}
        mode = 0
    else:
        mode = (
            getattr(_os, "RTLD_LOCAL", 0)
            | getattr(_os, "RTLD_NOW", 0)
            | getattr(_os, "RTLD_DEEPBIND", 0)  # Linux only, as in BlasHeaders.cc
        )
        kwargs = {}
    try:
        return _ctypes.CDLL(name, mode=mode, **kwargs)
    except OSError:
        return None


def _has_symbols(handle, symbols):
    return all(hasattr(handle, s) for s in symbols)


def native_math_libraries_found(system=None, open_library=None):
    """
    True if the compiled module will find a BLAS/LAPACK by itself.

    This is an approximation of MathLoader::LoadMathLibraries() for the case
    where DEVSIM_MATH_LIBS is not set: an MKL that provides the required
    symbols, or the default library names together providing them.
    """
    if system is None:
        system = _platform.system()
    if open_library is None:
        open_library = _open_library

    for name in _mkl_names(system):
        handle = open_library(name)
        if handle is not None and _has_symbols(handle, _REQUIRED_SYMBOLS):
            return True

    missing = set(_REQUIRED_SYMBOLS)
    for name in _default_names(system):
        handle = open_library(name)
        if handle is None:
            continue
        missing = {s for s in missing if not hasattr(handle, s)}
        if not missing:
            return True
    return False


def find_scipy_openblas_library(lib_dir, system=None):
    """
    Path of the scipy-openblas32 shared library in lib_dir, or None.

    The exact name differs between releases and platforms, so the directory
    is searched.  ``libscipy_openblas.<ext>`` is preferred when present.
    """
    ext = shared_library_extension(system)
    if ext is None or not lib_dir or not _os.path.isdir(lib_dir):
        return None
    if ext == "so":
        pattern = _re.compile(r"^(lib)?scipy_openblas.*\.so(\.\d+)*$")
    else:
        pattern = _re.compile(r"^(lib)?scipy_openblas.*\.%s$" % ext)
    names = sorted(n for n in _os.listdir(lib_dir) if pattern.match(n))
    names = [n for n in names if _os.path.isfile(_os.path.join(lib_dir, n))]
    if not names:
        return None
    for preferred in ("libscipy_openblas.%s" % ext, "scipy_openblas.%s" % ext):
        if preferred in names:
            return _os.path.join(lib_dir, preferred)
    return _os.path.join(lib_dir, names[0])


def _opted_out(environ):
    return environ.get(OPT_OUT_VARIABLE, "") not in ("", "0")


def select_scipy_openblas_env(
    environ=None,
    system=None,
    import_module=None,
    native_found=None,
):
    """
    Environment variables that make devsim use scipy-openblas32, or {}.

    Returns {} when the user set DEVSIM_MATH_LIBS, when the fallback is
    switched off, when a BLAS/LAPACK is found natively, or when
    scipy_openblas32 is not installed.  Nothing is modified here.
    """
    if environ is None:
        environ = _os.environ
    if system is None:
        system = _platform.system()
    if import_module is None:
        import_module = _importlib.import_module
    if native_found is None:
        native_found = native_math_libraries_found

    if _opted_out(environ) or "DEVSIM_MATH_LIBS" in environ:
        return {}
    if shared_library_extension(system) is None:
        return {}
    if native_found(system):
        return {}

    try:
        scipy_openblas32 = import_module("scipy_openblas32")
        lib_dir = scipy_openblas32.get_lib_dir()
    except (ImportError, AttributeError):
        return {}

    library = find_scipy_openblas_library(lib_dir, system)
    if library is None:
        return {}
    library = _os.path.normpath(library)  # get_lib_dir() may use "/" on Windows
    # DEVSIM_MATH_LIBS is a list; the separator is ';' on Windows, ':' elsewhere
    # (the same as os.pathsep), so a path containing it cannot be passed.
    if _os.pathsep in library:
        return {}

    return {
        "DEVSIM_MATH_LIBS": library,
        "DEVSIM_BLAS_PREFIX": BLAS_PREFIX,
        "DEVSIM_BLAS_SUFFIX": BLAS_SUFFIX,
    }


def apply_scipy_openblas_fallback(environ=None, **kwargs):
    """
    Set the environment if the fallback applies.

    Returns the previous values of the variables that were set (None for
    absent ones), to be given to restore_environment(), or None if nothing
    was changed.
    """
    if environ is None:
        environ = _os.environ
    new = select_scipy_openblas_env(environ=environ, **kwargs)
    if not new:
        return None
    previous = {k: environ.get(k) for k in new}
    environ.update(new)
    return previous


def restore_environment(previous, environ=None):
    """Undo apply_scipy_openblas_fallback(); the compiled module has read it."""
    if not previous:
        return
    if environ is None:
        environ = _os.environ
    for key, value in previous.items():
        if value is None:
            environ.pop(key, None)
        else:
            environ[key] = value
