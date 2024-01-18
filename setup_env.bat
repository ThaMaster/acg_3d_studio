@echo off

::
:: Setting up Build and Runtime environment for the vr library and examples
::

set A_CURR_DIR=%~d0%~p0

set args=%*


:: Setup visual studio path

REM Default is Visual Studio 2022 64 bit
if "%args%"=="" (
    set args="/VS2022"

)


set BUILD_ARCHITECHTURE=x64


call %A_CURR_DIR%\setup_vs.bat %args%
set STATUS=%ERRORLEVEL%
set CMAKE_GENERATOR_VERSION=%VS_GENERATOR_VERSION%


ECHO Generator %VS_GENERATOR_VERSION%
ECHO Architechture: %BUILD_ARCHITECHTURE%

pushd %A_CURR_DIR%
IF NOT "%STATUS%"=="0" (
  echo.
  echo *** No visual studio found ***
  goto error
)

set VR_PATH=%A_CURR_DIR%
set DEPENDENCY_PATH=%A_CURR_DIR%\installed


REM Add to path so .dll files can be found
set PATH=%PATH%;%DEPENDENCY_PATH%\bin;%VR_PATH%\bin
set LIB=%LIB%;%DEPENDENCY_PATH%\lib
set INCLUDE=%INCLUDE%;%DEPENDENCY_PATH%\include
goto exit

:error
exit /B 1

:exit
exit /B 0

