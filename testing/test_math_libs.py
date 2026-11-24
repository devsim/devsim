# SPDX-License-Identifier: Apache-2.0
#
# Pure Python tests for the scipy-openblas32 fallback in lib/devsim/_math_libs.py.
# They do not need the compiled devsim module and do not need scipy_openblas32.
#
#   python -m pytest testing/test_math_libs.py
#
import importlib.util
import os
import pathlib
import types

import pytest

_SOURCE = pathlib.Path(__file__).resolve().parent.parent / "lib" / "devsim"
_SPEC = importlib.util.spec_from_file_location(
    "devsim_math_libs_under_test", _SOURCE / "_math_libs.py"
)
ml = importlib.util.module_from_spec(_SPEC)
_SPEC.loader.exec_module(ml)

EXT = {"Windows": "dll", "Darwin": "dylib", "Linux": "so"}


def fake_scipy_openblas32(lib_dir):
    return types.SimpleNamespace(get_lib_dir=lambda: str(lib_dir))


def make_importer(module):
    calls = []

    def import_module(name):
        calls.append(name)
        if module is None or name != "scipy_openblas32":
            raise ImportError(name)
        return module

    import_module.calls = calls
    return import_module


def make_lib_dir(tmp_path, system, name=None):
    ext = EXT[system]
    name = name or "libscipy_openblas.%s" % ext
    (tmp_path / name).write_bytes(b"")
    return tmp_path


def never(system):
    raise AssertionError("native search must not run")


def not_found(system):
    return False


def found(system):
    return True


@pytest.mark.parametrize("system", ["Windows", "Darwin", "Linux"])
def test_sets_all_three_variables_when_nothing_found(tmp_path, system):
    lib_dir = make_lib_dir(tmp_path, system)
    env = ml.select_scipy_openblas_env(
        environ={},
        system=system,
        import_module=make_importer(fake_scipy_openblas32(lib_dir)),
        native_found=not_found,
    )
    assert env == {
        "DEVSIM_MATH_LIBS": os.path.join(str(lib_dir), "libscipy_openblas." + EXT[system]),
        "DEVSIM_BLAS_PREFIX": "scipy_",
        "DEVSIM_BLAS_SUFFIX": "_",
    }


def test_uses_the_actual_file_name_found(tmp_path):
    lib_dir = make_lib_dir(tmp_path, "Linux", "libscipy_openblas-0123abcd.so")
    # other files in the directory must be ignored
    (tmp_path / "libscipy_openblas.a").write_bytes(b"")
    (tmp_path / "scipy_openblas.pc").write_bytes(b"")
    env = ml.select_scipy_openblas_env(
        environ={},
        system="Linux",
        import_module=make_importer(fake_scipy_openblas32(lib_dir)),
        native_found=not_found,
    )
    assert env["DEVSIM_MATH_LIBS"] == str(lib_dir / "libscipy_openblas-0123abcd.so")


def test_windows_import_library_is_not_chosen(tmp_path):
    (tmp_path / "libscipy_openblas.lib").write_bytes(b"")
    assert ml.find_scipy_openblas_library(str(tmp_path), "Windows") is None
    (tmp_path / "scipy_openblas.dll").write_bytes(b"")
    assert ml.find_scipy_openblas_library(str(tmp_path), "Windows") == str(
        tmp_path / "scipy_openblas.dll"
    )


def test_exact_name_preferred(tmp_path):
    (tmp_path / "libscipy_openblas-aaa.dll").write_bytes(b"")
    (tmp_path / "libscipy_openblas.dll").write_bytes(b"")
    assert ml.find_scipy_openblas_library(str(tmp_path), "Windows") == str(
        tmp_path / "libscipy_openblas.dll"
    )


def test_user_math_libs_untouched(tmp_path):
    lib_dir = make_lib_dir(tmp_path, "Windows")
    importer = make_importer(fake_scipy_openblas32(lib_dir))
    environ = {"DEVSIM_MATH_LIBS": "mine.dll"}
    assert (
        ml.select_scipy_openblas_env(
            environ=environ,
            system="Windows",
            import_module=importer,
            native_found=never,
        )
        == {}
    )
    assert ml.apply_scipy_openblas_fallback(
        environ=environ,
        system="Windows",
        import_module=importer,
        native_found=never,
    ) is None
    assert environ == {"DEVSIM_MATH_LIBS": "mine.dll"}
    assert importer.calls == []


def test_empty_user_math_libs_counts_as_set(tmp_path):
    # An empty value is still "set" for std::getenv in the compiled module.
    lib_dir = make_lib_dir(tmp_path, "Windows")
    assert (
        ml.select_scipy_openblas_env(
            environ={"DEVSIM_MATH_LIBS": ""},
            system="Windows",
            import_module=make_importer(fake_scipy_openblas32(lib_dir)),
            native_found=never,
        )
        == {}
    )


@pytest.mark.parametrize("value", ["1", "yes", "true"])
def test_opt_out(tmp_path, value):
    lib_dir = make_lib_dir(tmp_path, "Windows")
    importer = make_importer(fake_scipy_openblas32(lib_dir))
    assert (
        ml.select_scipy_openblas_env(
            environ={"DEVSIM_NO_SCIPY_OPENBLAS": value},
            system="Windows",
            import_module=importer,
            native_found=never,
        )
        == {}
    )
    assert importer.calls == []


@pytest.mark.parametrize("value", ["", "0"])
def test_opt_out_zero_or_empty_means_enabled(tmp_path, value):
    lib_dir = make_lib_dir(tmp_path, "Windows")
    env = ml.select_scipy_openblas_env(
        environ={"DEVSIM_NO_SCIPY_OPENBLAS": value},
        system="Windows",
        import_module=make_importer(fake_scipy_openblas32(lib_dir)),
        native_found=not_found,
    )
    assert "DEVSIM_MATH_LIBS" in env


def test_native_blas_found_keeps_environment_and_skips_import(tmp_path):
    lib_dir = make_lib_dir(tmp_path, "Windows")
    importer = make_importer(fake_scipy_openblas32(lib_dir))
    assert (
        ml.select_scipy_openblas_env(
            environ={},
            system="Windows",
            import_module=importer,
            native_found=found,
        )
        == {}
    )
    # scipy_openblas32 is not even imported when MKL or OpenBLAS is there
    assert importer.calls == []


def test_not_installed():
    importer = make_importer(None)
    assert (
        ml.select_scipy_openblas_env(
            environ={},
            system="Windows",
            import_module=importer,
            native_found=not_found,
        )
        == {}
    )
    assert importer.calls == ["scipy_openblas32"]


def test_installed_but_library_missing(tmp_path):
    assert (
        ml.select_scipy_openblas_env(
            environ={},
            system="Windows",
            import_module=make_importer(fake_scipy_openblas32(tmp_path)),
            native_found=not_found,
        )
        == {}
    )
    assert (
        ml.select_scipy_openblas_env(
            environ={},
            system="Windows",
            import_module=make_importer(fake_scipy_openblas32(tmp_path / "missing")),
            native_found=not_found,
        )
        == {}
    )


def test_package_without_get_lib_dir():
    assert (
        ml.select_scipy_openblas_env(
            environ={},
            system="Windows",
            import_module=make_importer(types.SimpleNamespace()),
            native_found=not_found,
        )
        == {}
    )


def test_unknown_platform_does_nothing():
    assert (
        ml.select_scipy_openblas_env(
            environ={},
            system="FreeBSD",
            import_module=make_importer(None),
            native_found=never,
        )
        == {}
    )


def test_apply_and_restore(tmp_path):
    lib_dir = make_lib_dir(tmp_path, "Linux")
    # stray values are ignored by the compiled module without DEVSIM_MATH_LIBS,
    # but must be replaced while the fallback is active and put back afterwards
    environ = {"DEVSIM_BLAS_SUFFIX": "stray", "OTHER": "x"}
    previous = ml.apply_scipy_openblas_fallback(
        environ=environ,
        system="Linux",
        import_module=make_importer(fake_scipy_openblas32(lib_dir)),
        native_found=not_found,
    )
    assert environ["DEVSIM_BLAS_PREFIX"] == "scipy_"
    assert environ["DEVSIM_BLAS_SUFFIX"] == "_"
    assert environ["DEVSIM_MATH_LIBS"].endswith("libscipy_openblas.so")
    ml.restore_environment(previous, environ=environ)
    assert environ == {"DEVSIM_BLAS_SUFFIX": "stray", "OTHER": "x"}


def test_restore_none_is_a_no_op():
    environ = {"A": "1"}
    ml.restore_environment(None, environ=environ)
    assert environ == {"A": "1"}


class FakeLibrary:
    def __init__(self, symbols):
        self._symbols = set(symbols)

    def __getattr__(self, name):
        if name in self._symbols:
            return object()
        raise AttributeError(name)


ALL = ml._REQUIRED_SYMBOLS


def library_set(**libs):
    """open_library replacement; keys are names, values are symbol tuples."""
    opened = []

    def open_library(name):
        opened.append(name)
        if name in libs:
            return FakeLibrary(libs[name])
        return None

    open_library.opened = opened
    return open_library


def test_native_search_mkl_on_windows():
    opener = library_set(**{"mkl_rt.2.dll": ALL})
    assert ml.native_math_libraries_found("Windows", opener)
    assert opener.opened == ["mkl_rt.dll", "mkl_rt.1.dll", "mkl_rt.2.dll"]


def test_native_search_mkl_on_linux():
    assert ml.native_math_libraries_found("Linux", library_set(**{"libmkl_rt.so": ALL}))


def test_native_search_default_names_split_over_libraries():
    opener = library_set(
        **{"libopenblas.so": ("drotg_", "zrotg_"), "liblapack.so": ALL[:4]}
    )
    assert ml.native_math_libraries_found("Linux", opener)


def test_native_search_incomplete_libraries_do_not_count():
    opener = library_set(**{"libopenblas.dll": ("drotg_", "zrotg_")})
    assert not ml.native_math_libraries_found("Windows", opener)


def test_native_search_nothing_present():
    assert not ml.native_math_libraries_found("Windows", library_set())
    assert not ml.native_math_libraries_found("Darwin", library_set())


def test_native_search_no_mkl_on_macos():
    opener = library_set(**{"libopenblas.dylib": ALL})
    assert ml.native_math_libraries_found("Darwin", opener)
    assert not any("mkl" in n for n in opener.opened)
