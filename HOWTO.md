# HOWTO
## Update version number

To update the version number update the lines in these files:

* ``CMakeLists.txt``
```
ADD_DEFINITIONS(-DDEVSIM_VERSION_STRING=\"2.10.0\")
```
* ``dist/bdist_wheel/pyproject.toml``
```
version = "2.11.3"
```

## Update minimum python version
* ``dist/bdist_wheel/setup.py``
```
py_limited_api = "cp39"
```
* ``src/pythonapi/CMakeLists.txt``
```
target_compile_definitions(pythonapi_interpreter_py3 PRIVATE -DDEVSIM_MODULE_NAME=devsim_py3 -DPy_LIMITED_API=0x03090000)
```
* Additional Python Notes
  * [C API Stability](https://docs.python.org/3/c-api/stable.html)
