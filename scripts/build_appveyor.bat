::turn back off later
@echo on
@SETLOCAL EnableDelayedExpansion
SET BASEDIR=%cd%
SET PYTHON3_BIN=%2
SET TAG=%3

c:\msys64\usr\bin\pacman -Su --noconfirm bison flex git

IF "%1"=="msys" (
  SET BUILDDIR=msys
  SET PACKAGE_NAME="devsim_msys_%TAG%"
  SET USE_MSYS=true
  SET BASH_SCRIPT=%BASEDIR%\scripts\build_msys.sh
)

IF "%1"=="clangx64" (
  SET GENERATOR="Visual Studio 18 2026"
  SET AOPTION="x64"
  SET TOOLSET="ClangCL"
  SET BUILDDIR="clangwin64"
  SET USE_VISUAL_STUDIO=true
  SET PACKAGE_NAME="devsim_clangwin64_%TAG%"
  SET BASH_SCRIPT=%BASEDIR%\scripts\build_appveyor.sh
)


IF "%1"=="x64" (
  SET GENERATOR="Visual Studio 18 2026"
  SET AOPTION="x64"
  SET TOOLSET="v145"
  SET BUILDDIR=win64
  SET USE_VISUAL_STUDIO=true
  SET PACKAGE_NAME="devsim_win64_%TAG%"
  SET BASH_SCRIPT=%BASEDIR%\scripts\build_appveyor.sh
)

cd %BASEDIR%
IF DEFINED USE_VISUAL_STUDIO c:\msys64\usr\bin\bash %BASH_SCRIPT% %GENERATOR% %AOPTION% %TOOLSET% %BUILDDIR% %PYTHON3_BIN% %PACKAGE_NAME%
if %errorlevel% neq 0 exit /b %errorlevel%

IF DEFINED USE_MSYS c:\msys64\usr\bin\bash %BASH_SCRIPT% %PACKAGE_NAME%
if %errorlevel% neq 0 exit /b %errorlevel%
