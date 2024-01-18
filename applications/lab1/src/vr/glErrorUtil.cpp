

#include "vr/glErrorUtil.h"
#include <glad/glad.h>
//#include <GL/glew.h>
#include <sstream>
using namespace std;

using namespace vr;

char const* gl_error_string(GLenum const err) noexcept
{
  switch (err)
  {
    // opengl 2 errors (8)
  case GL_NO_ERROR:
    return "GL_NO_ERROR";

  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";

  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";

  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";

  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";

  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";

  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";

  case GL_TABLE_TOO_LARGE:
    return "GL_TABLE_TOO_LARGE";

    // opengl 3 errors (1)
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
  default:
    return "Unknown error";
  }
}

#ifndef NDEBUGXX
bool CheckErrorsGL(const std::string& location, ostream& ostr) 
{
	GLuint errnum;
	const char *errstr;
	while ((errnum = glGetError())) 
	{
		errstr = reinterpret_cast<const char *>(gl_error_string(errnum));
		ostr << errstr; 

		if(location.length()) ostr << " at " << location.c_str();		
		ostr << endl;

    return false;
	}
	return true;
}
#endif

std::string vr::stringify(const char* file, int line)
{
  std::ostringstream str;
  str << file << ":" << line;
  return str.str();
}
