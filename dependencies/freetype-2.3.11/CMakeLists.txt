cmake_minimum_required(VERSION 2.6)

PROJECT (freetype)

IF (CMAKE_SIZEOF_VOID_P MATCHES "8" AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_C_FLAGS "-fPIC")
  set(CMAKE_CXX_FLAGS "-fPIC")
ENDIF()

SET(BUILD_MODE STATIC)
SET(TARGET_NAME freetype)


#SET(LIBRARY_OUTPUT_PATH ${Gimpact_BINARY_DIR}/lib)

SET(CMAKE_DEBUG_POSTFIX "d")




INCLUDE_DIRECTORIES(
					${freetype_SOURCE_DIR}/include 
          # ${freetype_SOURCE_DIR}/src/base
					)



ADD_DEFINITIONS(-DFT2_BUILD_LIBRARY -D_CRT_SECURE_NO_WARNINGS)

SET( ft_files 
	src/autofit/autofit.c
	src/bdf/bdf.c
	src/cff/cff.c
	src/base/ftbase.c
	src/base/ftbitmap.c
	src/cache/ftcache.c
	src/base/ftdebug.c
	src/base/ftfstype.c
	src/base/ftgasp.c
	src/base/ftglyph.c
	src/gzip/ftgzip.c
	src/base/ftinit.c
	src/lzw/ftlzw.c
	src/base/ftstroke.c
	src/base/ftsystem.c
	src/smooth/smooth.c
	src/base/ftbbox.c
	src/base/ftmm.c
	src/base/ftmm.c
	src/base/ftpfr.c	
	src/base/ftsynth.c
	src/base/fttype1.c
	src/base/ftwinfnt.c
	src/pcf/pcf.c
	src/pfr/pfr.c
	src/psaux/psaux.c
	src/pshinter/pshinter.c
	src/psnames/psmodule.c
	src/raster/raster.c
	src/sfnt/sfnt.c
	src/truetype/truetype.c
	src/type1/type1.c
	src/cid/type1cid.c
	src/type42/type42.c
	src/winfonts/winfnt.c
)
	

IF (APPLE)
  SET(BUILD_MODE SHARED)
  SET(TARGET_NAME freetype)
  # SET(ft_files ${ft_files} src/base/ftmac.c builds/mac/ftmac.c)
  ADD_DEFINITIONS("-DDARWIN_NO_CARBON")
ENDIF()

# Requires that the build_windows.bat is executed prior to running cmake on this
LINK_DIRECTORIES( $ENV{TARGET_LIB_DIR} )

ADD_LIBRARY(${TARGET_NAME} ${BUILD_MODE} ${ft_files})


INSTALL(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin/ ARCHIVE DESTINATION lib LIBRARY DESTINATION lib )
INSTALL(DIRECTORY include DESTINATION . COMPONENT headers PATTERN .svn EXCLUDE)
