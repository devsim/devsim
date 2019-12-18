
:: SET_USE_CYGWIN is true for Visual Studio Builds
IF "%PLATFORM%"=="x64" SET CONDA_PATH=c:\Miniconda37-x64\Scripts\conda && SET BUILDDIR=win64 && SET USE_CYGWIN=true
IF "%PLATFORM%"=="x86" SET CONDA_PATH=c:\Miniconda37\Scripts\conda && SET BUILDDIR=win32 && SET_USE_CYGWIN=true

:: GET PREREQUISITES
IF DEFINED USE_CYGWIN (
  %CONDA_PATH% install --yes mkl mkl-devel mkl-include sqlite zlib
  appveyor DownloadFile https://cygwin.com/setup-x86.exe
  setup-x86.exe -qnNdO -R C:/cygwin -s http://cygwin.mirror.constant.com -l C:/cygwin/var/cache/setup -P flex -P bison -P zip
)

:: BUILD DEPENDENCIES
IF DEFINED USE_CYGWIN (
  cd %APPVEYOR_BUILD_FOLDER%\external
  c:\cygwin\bin\bash.exe ./build_superlu_appveyor.sh %PLATFORM%
  cd external\symdiff
  c:\cygwin\bin\bash.exe ../symdiff_appveyor.sh %PLATFORM%
  cd %BUILDDIR%
  cmake --build . --config Release -- /m /nologo /verbosity:minimal
)

:: BUILD DEVSIM
IF DEFINED USE_CYGWIN (
  cd %APPVEYOR_BUILD_FOLDER%
  c:\cygwin\bin\bash.exe scripts/setup_appveyor.sh %PLATFORM%
  cd %BUILDDIR%
  cmake --build . --config Release -- /m /v:m
)

:: PACKAGE DEVSIM
IF DEFINED USE_CYGWIN (
  cd %APPVEYOR_BUILD_FOLDER%\dist
  c:\cygwin\bin\bash.exe package_appveyor.sh devsim_%BUILDDIR%_%APPVEYOR_REPO_TAG_NAME%
)

