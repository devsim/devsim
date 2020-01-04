
@echo on
:: The following line is important for proper variable expansion
@SETLOCAL EnableDelayedExpansion
SET BASEDIR=%cd%
SET CONDA_BIN=%2
SET TAG=%3

call %CONDA_BIN% create -y -n devsim_build python=3.7
if %errorlevel% neq 0 exit /b %errorlevel%

call %CONDA_BIN% install -y -n devsim_build cmake mkl mkl-devel mkl-include sqlite zlib
if %errorlevel% neq 0 exit /b %errorlevel%

call %CONDA_BIN% activate devsim_build
if %errorlevel% neq 0 exit /b %errorlevel%

:: SET_USE_VISUALSTUDIO is true for Visual Studio Builds
IF "%1"=="msys" (
  SET BUILDDIR=msys
  SET PACKAGE_NAME="devsim_msys_%TAG%"
)

IF "%1"=="x64" (
  SET GENERATOR="Visual Studio 16 2019"
  SET AOPTION="x64"
  SET BUILDDIR=win64
  SET USE_VISUALSTUDIO=true
  SET PACKAGE_NAME="devsim_win64_%TAG%"
)

IF "%1"=="x86" (
  SET GENERATOR="Visual Studio 16 2019"
  SET AOPTION="Win32"
  SET BUILDDIR=win32
  SET USE_VISUALSTUDIO=true
  SET PACKAGE_NAME="devsim_win32_%TAG%"
)

SET PATH="c:\msys64\mingw64\bin;c:\msys64\usr\bin;%PATH%"

:: GET PREREQUISITES
IF DEFINED USE_VISUALSTUDIO (

  :: BUILD DEPENDENCIES
  cd %BASEDIR%\external

  c:\msys64\usr\bin\bash ./build_superlu_appveyor.sh %GENERATOR% %AOPTION% %BUILDDIR%
  if %errorlevel% neq 0 exit /b %errorlevel%

  cd %BASEDIR%\external\symdiff

  c:\msys64\usr\bin\bash ../symdiff_appveyor.sh %GENERATOR% %AOPTION% %BUILDDIR% %CONDA_PREFIX%
  if %errorlevel% neq 0 exit /b %errorlevel%

  cd %BUILDDIR%
  cmake --build . --config Release -- /m /nologo /verbosity:minimal
  if %errorlevel% neq 0 exit /b %errorlevel%

:: BUILD DEVSIM
  cd %BASEDIR%
  c:\msys64\usr\bin\bash scripts/setup_appveyor.sh %GENERATOR% %AOPTION% %BUILDDIR% %CONDA_PREFIX%
  if %errorlevel% neq 0 exit /b %errorlevel%

  cd %BUILDDIR%
  cmake --build . --config Release -- /m /v:m
  if %errorlevel% neq 0 exit /b %errorlevel%

  :: PACKAGE DEVSIM
  cd %BASEDIR%\dist
  c:\msys64\usr\bin\bash package_appveyor.sh %PACKAGE_NAME%
  if %errorlevel% neq 0 exit /b %errorlevel%

) ELSE (

  SET PATH=c:\msys64\mingw64\bin;c:\msys64\usr\bin;%PATH%

  call %CONDA_BIN% install -y -n devsim_build boost
  if %errorlevel% neq 0 exit /b %errorlevel%

  cd %BASEDIR%
  c:\msys64\usr\bin\bash %BASEDIR%\scripts\build_msys.sh %PACKAGE_NAME%
  if %errorlevel% neq 0 exit /b %errorlevel%
)

