#
# This CMakeLists.txt will automatically add any subdirectory to the list of projects that should 
# be built. Just copy one of the subdirectories (for example objViewer) to a new name


# Macro that will make a list of all subdirectories
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  # Filter away any unwanted directory
  list(FILTER dirlist EXCLUDE REGEX ".*CMake.*")
  SET(${result} ${dirlist})
ENDMACRO()


SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# For each subdirectory add it as a subdirectory
FOREACH(subdir ${SUBDIRS})
  MESSAGE("Building subdirectory: ${subdir}")
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()
