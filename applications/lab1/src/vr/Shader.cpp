
#include <vr/Shader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vr/FileSystem.h>

using namespace vr;

namespace
{
  std::string readFileContent(const std::string& path)
  {
    std::string f_path = vr::FileSystem::findFile(path);
    if (f_path.empty())
      throw std::runtime_error("Unable to locate shader file: " + path);

    std::ifstream textFile;
    textFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    textFile.open(f_path);

    std::stringstream textStream;

    // read file's buffer contents into streams
    textStream << textFile.rdbuf();

    textFile.close();

    return textStream.str();
  }
}

int Shader::createShader(const GLchar *source, GLenum shader_type, const char *description)
{
  // vertex shader
  GLuint vertex = glCreateShader(shader_type);
  glShaderSource(vertex, 1, &source, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, description);
  return vertex;

}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) : m_valid(true), vPath(vertexPath), fPath(fragmentPath)
{
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;  
  
  try
  {
    vertexCode = readFileContent(vertexPath);
    fragmentCode = readFileContent(fragmentPath);

    // if geometry shader path is present, also load a geometry shader
    if (!geometryPath.empty())
      geometryCode = readFileContent(geometryPath);
  }
  catch (std::ifstream::failure& e)
  {
    std::cerr << "ERROR reading shader file: " << e.what() << std::endl;
    m_valid = false;
  }
  catch (std::runtime_error& e)
  {
    std::cerr << "ERROR reading shader file: " << e.what() << std::endl;
    m_valid = false;
  }

  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();
  
  // 2. compile shaders
  unsigned int vertex, fragment;
  
  // vertex shader
  vertex = createShader(vShaderCode, GL_VERTEX_SHADER, "VERTEX");
  
  // fragment Shader
  fragment = createShader(fShaderCode, GL_FRAGMENT_SHADER, "FRAGMENT");
  
  // if geometry shader is given, compile geometry shader
  unsigned int geometry = 0;
  if (!geometryPath.empty())
    geometry = createShader(geometryCode.c_str(), GL_GEOMETRY_SHADER, "GEOMETRY");
  
  // shader Program
  m_programID = glCreateProgram();
  glAttachShader(m_programID, vertex);
  glAttachShader(m_programID, fragment);
  if (!geometryPath.empty())
    glAttachShader(m_programID, geometry);
  glLinkProgram(m_programID);
  checkCompileErrors(m_programID, "PROGRAM");
  
  // delete the shaders as they're linked into our program now and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (!geometryPath.empty())
    glDeleteShader(geometry);
}

void Shader::use()
{
  glUseProgram(m_programID);
}

GLint getLocation(GLuint program, const char* name)
{
  auto loc = glGetUniformLocation(program, name);
  if (loc == -1)
  {
    std::cerr << "Could not bind uniform " << name << std::endl;
  }
  return loc;
}

void Shader::setBool(const std::string& name, bool value) const
{
  glUniform1i(getLocation(m_programID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
  glUniform1i(getLocation(m_programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
  glUniform1f(getLocation(m_programID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
  glUniform2fv(getLocation(m_programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
  glUniform2f(getLocation(m_programID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
  glUniform3fv(getLocation(m_programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
  glUniform3f(getLocation(m_programID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
  glUniform4fv(getLocation(m_programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
  glUniform4f(getLocation(m_programID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
  glUniformMatrix2fv(getLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
  glUniformMatrix3fv(getLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
  glUniformMatrix4fv(getLocation(m_programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setIntVector(const std::string& name, const std::vector<int>& vector)
{
  auto loc = getLocation(m_programID, name.c_str());
  glUniform1iv(loc, (GLsizei)vector.size(), vector.data());

}

bool Shader::valid() const
{
  return m_valid;
}


void Shader::checkCompileErrors(GLuint shader, std::string type)
{
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      m_valid = false;
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      m_valid = false;

      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
}

GLint Shader::getAttribute(const std::string& attributeName) const
{
  auto loc = glGetAttribLocation(m_programID, attributeName.c_str());
  if (loc == -1)
  {
    std::cerr << "Error getting attribute: " << attributeName << std::endl;
  }

  return loc;
}

GLint Shader::program() const
{
  return m_programID;
}

