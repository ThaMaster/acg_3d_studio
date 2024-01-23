
#pragma once
#include <glad/glad.h>
#include <iostream>


char const* gl_error_string(GLenum const err) noexcept;

namespace vr
{
  std::string stringify(const char* file, int line);
}

// A simple OpenGL error checking routine. This compiles away
// to a no-op inline method if the NDEBUG preprocessor symbol
// is defined during compilation.
//
// - The first parameter (optional) is a string that can be 
//   used to indicate the location where the error check occurs.
//
// - The second parameter determines the destination of the error
//   message. It defaults to cerr, but could also be a file.
#ifndef NDEBUGXX
bool CheckErrorsGL( const std::string& location = "", 
				    std::ostream& ostr = std::cerr );
#else
inline bool CheckErrorsGL( const std::string& = "", 
						   std::ostream& /*ostr*/ = std::cerr ) 
{ return true; }
#endif 

#define DO_GL_AND_CHECK(A) A; CheckErrorsGL(#A)

#define CHECK_GL_ERROR_LINE_FILE() \
  CheckErrorsGL(vr::stringify(__FILE__, __LINE__))

#define CHECK_FRAMEBUFFER_STATUS() \
{\
  GLenum status; \
  status = glCheckFramebufferStatus(GL_FRAMEBUFFER); \
  /*std::cerr <<  "%x" << std::endl, status); */\
  switch(status) { \
  case GL_FRAMEBUFFER_COMPLETE: \
  /*std::cerr << "framebuffer complete!" << std::endl); */\
  break; \
  case GL_FRAMEBUFFER_UNSUPPORTED: \
  std::cerr << "framebuffer GL_FRAMEBUFFER_UNSUPPORTED" << std::endl);\
  /* you gotta choose different formats */ \
  assert(0); \
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: \
  std::cerr << "framebuffer INCOMPLETE_ATTACHMENT" << std::endl);\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: \
  std::cerr << "framebuffer FRAMEBUFFER_MISSING_ATTACHMENT" << std::endl);\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: \
  std::cerr << "framebuffer FRAMEBUFFER_DIMENSIONS" << std::endl);\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS: \
  std::cerr << "framebuffer INCOMPLETE_FORMATS" << std::endl);\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: \
  std::cerr << "framebuffer INCOMPLETE_DRAW_BUFFER" << std::endl);\
  break; \
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: \
  std::cerr << "framebuffer INCOMPLETE_READ_BUFFER" << std::endl);\
  break; \
  case GL_FRAMEBUFFER_BINDING: \
  std::cerr << "framebuffer BINDING" << std::endl);\
  break; \
  /* 
case GL_FRAMEBUFFER_STATUS_ERROR: \
std::cerr << "framebuffer STATUS_ERROR" << std::endl);\
break; \
*/ \
  default: \
           /* programming error; will fail on all hardware */ \
           assert(0); \
}\
}
