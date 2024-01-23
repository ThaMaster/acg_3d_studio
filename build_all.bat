::
:: To build a specific library add it as the argument to this script.
:: The matching label will be used:
:: Example:
::     build_all assimp
::
:: Not that there might be dependencies between the libraries.

@echo off
set A_CURR_DIR=%~d0%~p0

IF not errorlevel 0 goto error

IF NOT EXIST "%A_CURR_DIR%dependencies" (
  echo.
  echo.
  echo *** %A_CURR_DIR%dependencies is missing. Please download and unzip to that directory before running this script again
  goto error
)


set LABEL=%1


mkdir "%A_CURR_DIR%\installed\include\rapidxml\"
copy "%A_CURR_DIR%\dependencies\rapidxml-1.13\*.hpp" "%A_CURR_DIR%\installed\include\rapidxml\"
copy "%A_CURR_DIR%\dependencies\stb\*.h" "%A_CURR_DIR%\installed\include"


if NOT "%LABEL%"=="" goto %1

IF "%CMAKE_GENERATOR_VERSION%"=="" (
  echo *** setup_env.bat has not been setup correctly
  goto error
)

:: Build glfw
:glfw
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\glfw"
pushd "%A_CURR_DIR%\build\glfw"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" -A %BUILD_ARCHITECHTURE%  -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DBUILD_SHARED_LIBS=OFF "%A_CURR_DIR%\dependencies\glfw-3.3.9"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glfw.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glfw.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error

popd
IF NOT "%LABEL%"=="" goto exit


:glm
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\glm"
pushd "%A_CURR_DIR%\build\glm"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" -A %BUILD_ARCHITECHTURE%   -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" "%A_CURR_DIR%\dependencies\glm"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glm.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" glm.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error

popd
IF NOT "%LABEL%"=="" goto exit

:: ZLIB
:zlib
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\zlib"
pushd "%A_CURR_DIR%\build\zlib"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" -A %BUILD_ARCHITECHTURE%  -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed"  "%A_CURR_DIR%\dependencies\zlib-1.2.11" 
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" zlib.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" zlib.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit


:assimp
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\assimp"
pushd "%A_CURR_DIR%\build\assimp"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" -A %BUILD_ARCHITECHTURE%   -DCMAKE_CXX_FLAGS=/EHsc -DASSIMP_BUILD_NO_XFILE_EXPORTER=1 -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=off -DASSIMP_BUILD_X_IMPORTER=off -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" -DZLIB_INCLUDE_DIR="%A_CURR_DIR%\installed\include" "%A_CURR_DIR%\dependencies\assimp-5.3.1"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" assimp.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" assimp.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit

:freetype
mkdir "%A_CURR_DIR%\build"
mkdir "%A_CURR_DIR%\build\freetype-2.3.11"
pushd "%A_CURR_DIR%\build\freetype-2.3.11"
IF not errorlevel 0 goto error
cmake -G "%CMAKE_GENERATOR_VERSION%" -A %BUILD_ARCHITECHTURE%     -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" "%A_CURR_DIR%\dependencies\freetype-2.3.11"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" freetype.sln /release /WIN64
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" freetype.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error
popd
IF NOT "%LABEL%"=="" goto exit


:vrlib
set FREETYPE_DIR="%A_CURR_DIR%"\installed\



cmake -G "%CMAKE_GENERATOR_VERSION%" -A %BUILD_ARCHITECHTURE%    -DCMAKE_INSTALL_PREFIX="%A_CURR_DIR%\installed" -DZLIB_INCLUDE_DIR="%A_CURR_DIR%\installed\include" "%A_CURR_DIR%\"
IF not errorlevel 0 goto error

call "%A_CURR_DIR%\build" vrlib.sln /release /WIN64
IF not errorlevel 0 goto error

:install

call "%A_CURR_DIR%\build" vrlib.sln /release /WIN64 /INSTALL
IF not errorlevel 0 goto error


goto exit

:error
echo **** An error occurred 
exit /b 1

:exit
exit /b 0
