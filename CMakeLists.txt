cmake_minimum_required(VERSION 3.21)

project(vrlib)


# Name of whole project (only useful in visualstudio)

SET(OpenGL_GL_PREFERENCE "GLVND")

SET(VRLIB_MAJOR_VERSION 2)
SET(VRLIB_MINOR_VERSION 0)


IF(WIN32)
ELSE()
  SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
ENDIF(WIN32)

#if (COMMAND cmake_policy)
#  cmake_policy(SET CMP0003 OLD)
#endif (COMMAND cmake_policy)




# Path to other .cmake files
SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

SET( CMAKE_MODULE_PATH "${CMAKE_CURRENT_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")


# FInd a few useful packages
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLFW3 REQUIRED)
FIND_PACKAGE(Freetype)
SET( GLM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm")

FIND_PACKAGE(GLM REQUIRED)

FIND_PACKAGE(Assimp REQUIRED)

FIND_PACKAGE(ZLIB REQUIRED)


IF(NOT WIN32)
  FIND_PACKAGE(Threads) 
ENDIF()

# Did we find them?
IF(NOT GLFW3_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate GLFW3, download and install")
ENDIF()

IF(NOT FREETYPE_FOUND)
  MESSAGE(WARNING " Building without Freetype. Unable to locate FREETYPE, download and install")
  SET(FREETYPE_LIBRARY "")
ELSE()
  ADD_DEFINITIONS("-DHAVE_FREETYPE")
ENDIF()

# Did we find them?
IF(NOT ZLIB_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate ZLIB, download and install")
ENDIF()

# Did we find them?
IF(NOT GLM_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate GLM, download and install")
ENDIF(NOT GLM_FOUND)

# Did we find them?
IF(NOT ASSIMP_FOUND)
  MESSAGE(FATAL_ERROR " Unable to locate Assimp, download and install")
ENDIF()


# Set the default include directories
INCLUDE_DIRECTORIES(
  ${ZLIB_INCLUDE_DIR}
  ${${TARGET_NAME}_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
)

IF (FREETYPE_FOUND)
  INCLUDE_DIRECTORIES(
    ${FREETYPE_INCLUDE_DIR_ft2build}
    ${FREETYPE_INCLUDE_DIR_ft2build}/freetype2
  )
ENDIF()

set(X_LIBS ${CMAKE_THREAD_LIBS_INIT} dl Xcursor Xxf86vm Xinerama Xrandr Xi X11)

# Go through subdirectories
ADD_SUBDIRECTORY(applications)

