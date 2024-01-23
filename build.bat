@echo off
set ARGS="%*"

set BUILD_STATUS=0

set COMMAND=
set CONFIG=
set PROJ=
set FILENAME=%1


set argc=0
for %%a in (%*) do set /a argc+=1

echo %argc%

:: IS /build specified?
echo %ARGS% | FIND /I "/BU" >NUL
IF NOT ERRORLEVEL 1 set COMMAND=BUILD

set EXEC=msbuild

set BUILD_BUILDTYPE=x64

set BUILD_COMMAND=BUILD

:: IS /clean specified?
echo %ARGS% | FIND /I "/CL" >NUL
IF NOT ERRORLEVEL 1 set BUILD_COMMAND=CLEAN

echo %ARGS% | FIND /I "/BUI" >NUL
IF NOT ERRORLEVEL 1 set BUILD_COMMAND=BUILD

echo %ARGS% | FIND /I "/INS" >NUL
IF NOT ERRORLEVEL 1 set BUILD_COMMAND=INSTALL

echo %ARGS% | FIND /I "/PACK" >NUL
IF NOT ERRORLEVEL 1 set BUILD_COMMAND=PACKAGE

:: IS /rebuild specified?
echo %ARGS% | FIND /I "/REBU" >NUL
IF NOT ERRORLEVEL 1 set BUILD_COMMAND=REBUILD


set CONFIGURATION=

:: Which config should be used
echo %ARGS% | FIND /I "/min" >NUL
IF NOT ERRORLEVEL 1 set CONFIGURATION=MinSizeRel

:: Which config should be used
echo %ARGS% | FIND /I "/rele" >NUL
IF NOT ERRORLEVEL 1 set CONFIGURATION=Release

:: Which config should be used
echo %ARGS% | FIND /I "/RelW" >NUL
IF NOT ERRORLEVEL 1 set CONFIGURATION=RelWithDebInfo

:: Which config should be used
echo %ARGS% | FIND /I "/Debu" >NUL
IF NOT ERRORLEVEL 1 set CONFIGURATION=Debug

:: Check for custom configuration
echo %ARGS% | FIND /I "/CustomConfiguration=" >NUL
IF NOT ERRORLEVEL 1 (
  :looparguments
  IF NOT "%1"=="" (
    IF "%1"=="/CustomConfiguration" (
      SET CONFIGURATION=%2
      SHIFT
    )
    SHIFT
    GOTO :looparguments
  )
)

echo %FILENAME% | FIND /I ".vcxproj" >NUL
set STATUS=%ERRORLEVEL%
IF "%STATUS%"== "0" (
  goto skip_sln
)


echo %FILENAME% | FIND /I ".sln" >NUL
set STATUS=%ERRORLEVEL%
IF "%STATUS%"== "1" (
  set FILENAME=%FILENAME%.sln
)

:skip_sln

set USEENV=1
echo %ARGS% | FIND /I "/NOUSEENV" >NUL
IF NOT ERRORLEVEL 1 set USEENV=0

set BUILD_STATUS=1
if NOT EXIST %FILENAME% (
  echo ***
  echo *** Solution %FILENAME% doesn't exist
  echo ***
  goto exit
)
set BUILD_STATUS=0



echo %argc%
:: Only a solution specified, open visual studio with it.
if "%argc%"=="1" (
  devenv /useenv %FILENAME%
  set BUILD_STATUS=%ERRORLEVEL%
  goto exit
)

:: No arguments, just start visual studio
if "%argc%"=="0" (
  devenv /useenv
  set BUILD_STATUS=%ERRORLEVEL%
  goto exit

)
echo Filename       : %FILENAME%
echo Configuration  : %CONFIGURATION%
echo Command        : %BUILD_COMMAND%
echo Build type     : %BUILD_BUILDTYPE%
if "%CONFIGURATION%"=="" (
  echo "Missing configuration: /RELEASE or /DEBUG or /RELWITHDEBINFO"
  goto error_argument
)


if "%BUILD_COMMAND%" == "" (
  echo.
  echo *** Error missing build command: /BUILD or /REBUILD or /CLEAN
  echo.
  goto error
)

 call :getPath "!FILENAME!" install_path

::echo devenv %FILENAME% /BUILD  "%CONFIGURATION%|%BUILD_BUILDTYPE%" /project INSTALL
:: devenv %FILENAME% /BUILD  "%CONFIGURATION%|%BUILD_BUILDTYPE%" /project INSTALL

:: Use msbuild instead of devenv
if "%BUILD_COMMAND%"=="INSTALL" (
  echo msbuild  /p:configuration=%CONFIGURATION% /p:Platform=%BUILD_BUILDTYPE%  %install_path%INSTALL.vcxproj /m
  msbuild  /p:configuration=%CONFIGURATION%  /p:Platform=%BUILD_BUILDTYPE%  %install_path%INSTALL.vcxproj /m
)

set BUILD_STATUS=%ERRORLEVEL%
if "%BUILD_COMMAND%"=="INSTALL" goto exit


if "%BUILD_COMMAND%"=="PACKAGE" (
  echo msbuild  /p:configuration=%CONFIGURATION% /p:Platform=%BUILD_BUILDTYPE%  %install_path%install\win32\PACKAGE_InnoSetup.vcxproj /m
  msbuild  /p:configuration=%CONFIGURATION%  /p:Platform=%BUILD_BUILDTYPE%  %install_path%install\win32\PACKAGE_InnoSetup.vcxproj /m
)
set BUILD_STATUS=%ERRORLEVEL%
if "%BUILD_COMMAND%"=="PACKAGE" goto exit

set USEENVFLAG=
if %USEENV% EQU 1 set USEENVFLAG=/p:useEnv=true
if "%EXEC%"=="msbuild" (
  echo f: %USEENVFLAG%
  echo msbuild %FILENAME% /p:configuration=%CONFIGURATION% /target:%BUILD_COMMAND% /p:Platform=%BUILD_BUILDTYPE% %USEENVFLAG%
  msbuild %FILENAME% /p:configuration=%CONFIGURATION% /target:%BUILD_COMMAND% /p:Platform=%BUILD_BUILDTYPE% %USEENVFLAG% /m
) 

set USEENVFLAG=
if %USEENV% EQU 1 set USEENVFLAG=/useenv
if NOT "%EXEC%"=="msbuild" (
  echo Command: buildconsole %FILENAME%  %USEENVFLAG% /%BUILD_COMMAND% /cfg="%CONFIGURATION%|%BUILD_BUILDTYPE%" /UseMSBuild
  buildconsole %FILENAME%  %USEENVFLAG% /%BUILD_COMMAND% /cfg="%CONFIGURATION%|%BUILD_BUILDTYPE%" /UseMSBuild
)
set BUILD_STATUS=%ERRORLEVEL%

echo FILENAME: %FILENAME%
goto exit

IF %BUILD_STATUS% NEQ 0 (
  echo Error building %FILENAME%
  goto error_build
)

goto exit



:error_build
goto exit


:error_argument
echo off
echo.
echo."Flags/arguments are not case sensitive /BUILD == /build"
echo.
echo." Syntax : "
echo."  BUILD SOLUTION[.sln] [project] [Debug|RelWithDebInfo|Release|*] [flags]"
echo."  Flags: [/BUILD|/CLEAN|/REBUILD|/I|/WIN64]"
echo.""
echo." Examples:"
echo.""
echo." //Start visual studio with specified solution:"
echo." BUILD test.sln"
echo.""
echo." // Build viewer in test.sln in all configs"
echo." BUILD test.sln debug /BUILD"
echo.""
echo.""
echo." // Clean project viewer in test.sln using incredibuild"
echo." BUILD test viewer /CLEAN /I "
echo." "
set BUILD_STATUS=1
goto exit

:getPath
set "%2=%~dp1"
exit /b

:exit

exit /b %BUILD_STATUS%