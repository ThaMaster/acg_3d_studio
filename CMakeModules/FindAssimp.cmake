# Locate assimp library
#
# sets:
# ASSIMP_FOUND             - "YES" if everything is found, otherwise "NO"
# ASSIMP_INCLUDE_DIR       - set to where headers for assimp is located
# ASSIMP_LIBRARY_DIR       - path to assimp libraries
# ASSIMP_LIBRARIES         - libraries for assimp
# ASSIMP_LIBRARIES_RELEASE - libraries for assimp(release version)
# ASSIMP_LIBRARIES_DEBUG   - libraries for assimp (debug version)


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ASSIMP_ARCHITECTURE "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(ASSIMP_ARCHITECTURE "32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

find_path(ASSIMP_INCLUDE_DIR
  NAMES
  assimp/anim.h
  HINTS
  $ENV{DEPENDENCY_PATH}/include
  )

set(ASSIMP_LIBRARY_NAME "assimp")

if(WIN32)

  # This should take care of the versioning
  if(MSVC_TOOLSET_VERSION)
    set(MSVC_PREFIX "vc${MSVC_TOOLSET_VERSION}")
    set(ASSIMP_MSVC_VERSION ${MSVC_PREFIX})
  else()
    message(FATAL_ERROR "Unable to determine the version of Visual Studio!")
  endif()

  set(ASSIMP_LIBRARY_NAME ${ASSIMP_LIBRARY_NAME}-${ASSIMP_MSVC_VERSION}-mt)

  find_path(ASSIMP_LIBRARY_DIR
    NAMES
    ${ASSIMP_LIBRARY_NAME}.lib
    HINTS
    $ENV{DEPENDENCY_PATH}/lib
    )

  find_library(ASSIMP_LIBRARIES_RELEASE ${ASSIMP_LIBRARY_NAME}.lib    PATHS ${ASSIMP_LIBRARY_DIR})
  find_library(ASSIMP_LIBRARIES_DEBUG   ${ASSIMP_LIBRARY_NAME}d.lib   PATHS ${ASSIMP_LIBRARY_DIR})

  if (ASSIMP_LIBRARIES_DEBUG)
    set(ASSIMP_LIBRARIES
      optimized ${ASSIMP_LIBRARIES_RELEASE}
      debug     ${ASSIMP_LIBRARIES_DEBUG}
    )
  else()
    set(ASSIMP_LIBRARIES optimized ${ASSIMP_LIBRARIES_RELEASE})
  endif()


else()

  find_library(ASSIMP_LIBRARIES_RELEASE ${ASSIMP_LIBRARY_NAME}    PATHS $ENV{DEPENDENCY_PATH}/lib)
  find_library(ASSIMP_LIBRARIES_DEBUG   ${ASSIMP_LIBRARY_NAME}d   PATHS $ENV{DEPENDENCY_PATH}/lib)

if (ASSIMP_LIBRARIES_DEBUG)
    set(ASSIMP_LIBRARIES
      optimized ${ASSIMP_LIBRARIES_RELEASE}
      debug     ${ASSIMP_LIBRARIES_DEBUG}
    )
  else()
    set(ASSIMP_LIBRARIES optimized ${ASSIMP_LIBRARIES_RELEASE})
  endif()
    
endif()


if(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)
  set(ASSIMP_FOUND YES)
else()
  set(ASSIMP_FOUND NO)
endif()

if(NOT ASSIMP_FOUND AND ASSIMP_FIND_REQUIRED)
  message(FATAL_ERROR "Could not find asset importer library")
endif()
