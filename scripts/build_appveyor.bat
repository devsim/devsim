
@echo off
:: The following line is important for proper variable expansion
@SETLOCAL EnableDelayedExpansion
SET BASEDIR=%cd%
SET CONDA_BIN=%2
SET TAG=%3

c:\msys64\usr\bin\pacman -Su --noconfirm rsync zip bison flex git

:: now opt for explicit dll load of mkl
call %CONDA_BIN% create -y -n python3_devsim_build python=3 cmake sqlite zlib
if %errorlevel% neq 0 exit /b %errorlevel%

call %CONDA_BIN% activate python3_devsim_build
if %errorlevel% neq 0 exit /b %errorlevel%

IF "%1"=="msys" (
  SET BUILDDIR=msys
  SET PACKAGE_NAME="devsim_msys_%TAG%"
  SET USE_MSYS=true
  SET BASH_SCRIPT=%BASEDIR%\scripts\build_msys.sh
)

IF "%1"=="x64" (
  SET GENERATOR="Visual Studio 17 2022"
  SET AOPTION="x64"
  SET BUILDDIR=win64
  SET USE_VISUAL_STUDIO=true
  SET PACKAGE_NAME="devsim_win64_%TAG%"
  SET BASH_SCRIPT=%BASEDIR%\scripts\build_appveyor.sh
)

IF "%1"=="x86" (
  SET GENERATOR="Visual Studio 17 2022"
  SET AOPTION="Win32"
  SET BUILDDIR=win32
  SET USE_VISUAL_STUDIO=true
  SET PACKAGE_NAME="devsim_win32_%TAG%"
  SET BASH_SCRIPT=%BASEDIR%\scripts\build_appveyor.sh
)

cd %BASEDIR%
IF DEFINED USE_VISUAL_STUDIO c:\msys64\usr\bin\bash %BASH_SCRIPT% %GENERATOR% %AOPTION% %BUILDDIR% %CONDA_PREFIX% %PACKAGE_NAME%
if %errorlevel% neq 0 exit /b %errorlevel%

IF DEFINED USE_MSYS c:\msys64\usr\bin\bash %BASH_SCRIPT% %PACKAGE_NAME%
if %errorlevel% neq 0 exit /b %errorlevel%

