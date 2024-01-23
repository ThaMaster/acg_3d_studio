@echo off

set A_CURR_DIR=%~d0%~p0


setlocal EnableDelayedExpansion




set args="%*"

:: Hard code 64 bit
set args=%args% /WIN64



echo %args% | find /i "/VS2017" >NUL
if not errorlevel 1 (
  set version_lower=15.0
  set version_upper=16.0
  set generator_version=Visual Studio 15
)

echo %args% | find /i "/VS2019" >NUL
if not errorlevel 1 (
  set version_lower=16.0
  set version_upper=17.0
  set generator_version=Visual Studio 16
)

echo %args% | find /i "/VS2022" >NUL
if not errorlevel 1 (
  set version_lower=17.0
  set version_upper=18.0
  set generator_version=Visual Studio 17
)

:: Is /WIN64 specified?
set vs_build_type=x86
echo %args% | find /i "/WIN64" > nul
if not errorlevel 1 (
  set vs_build_type=amd64
)

:: Is /x64 specified?
echo %args% | find /i "/x64" > nul
if not errorlevel 1 (
  set vs_build_type=amd64
)


:: Looping over all installed Visual Studio versions.
::    %%i = "installationVersion" or "installationPath"
::    %%j =       <number>                <path>
for /f "usebackq tokens=1* delims=: " %%i in (`call "%A_CURR_DIR%\vswhere.exe" -legacy -all`) do (
  if /i "%%i"=="installationVersion" (
    set installed_version=%%j

    :: Testing version range such that [lower, upper).
    if !installed_version! geq !version_lower! (
      if not !installed_version! geq !version_upper! (
        goto version_found
      )
    )
  )

  :: Assumes "installationPath" comes before "installationVersion".
  if /i "%%i"=="installationPath" (
    set vs_installation_path=%%j
  )
)

:: We didn't find the specified version. Reset vs_installation_path.
set vs_installation_path=

echo.
echo Unable to find installation of: %args%
echo Visual Studio installations found:
for /f "usebackq tokens=1* delims=: " %%i in (`call "%A_CURR_DIR%\vswhere.exe" -legacy -all`) do (
  if /i "%%i"=="installationPath" (
    echo     %%i: %%j
  )
  if /i "%%i"=="installationVersion" (
    echo     %%i: %%j
  )
)

goto done

:version_found

echo.
echo Visual Studio version range:
echo     ^[!version_lower!, !version_upper!^)

:done

endlocal & (
    set "VS_INSTALLATION_PATH=%vs_installation_path%"
    set "VS_BUILD_TYPE=%vs_build_type%"
  	set "VS_GENERATOR_VERSION=%generator_version%"
  )

if "%VS_INSTALLATION_PATH%"=="" (
  goto exit
)

if %VS_BUILD_TYPE%==amd64 (
  :: For VS2013.
  set PreferredToolArchitecture=x64
)

:: VS2013 and VS2015: vs_installation_path/VC/vcvarsall.bat
:: VS2017:            vs_installation_path/VC/Auxiliary/Build/vcvarsall.bat

set VS_VCVARSALL_FILE="%VS_INSTALLATION_PATH%\VC\vcvarsall.bat"
if not exist %VS_VCVARSALL_FILE% (
  set VS_VCVARSALL_FILE="%VS_INSTALLATION_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
)

if not exist %VS_VCVARSALL_FILE% (
  echo. ERROR: Unable to find vcvarsall.bat!
  goto exit
)

echo Visual Studio installation path:
echo     %VS_INSTALLATION_PATH%
echo vcvarsall.bat used:
echo     %VS_VCVARSALL_FILE%
echo.

call %VS_VCVARSALL_FILE% %VS_BUILD_TYPE%

echo.

set VC_INITIALIZED=1

:exit
