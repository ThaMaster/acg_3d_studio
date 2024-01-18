#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


namespace vr
{
  /// Class that encapsulates the use of shaders in OpenGL.
  class Shader
  {
  public:

    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

    /// \return true if the shader is valid
    bool valid() const;

    /// \return the program id.
    GLint program() const;

    /// Activate the shader
    void use();

    /**
    \param attributeName - Name of the attribute.
    \return the identifier associated to the given attribute name. -1 if it fails.
    */
    GLint getAttribute(const std::string& attributeName) const;

    /// Set a named uniform of type bool
    void setBool(const std::string& name, bool value) const;
    
    /// Set a named uniform of type int
    void setInt(const std::string& name, int value) const;
    
    /// Set a named uniform of type float
    void setFloat(const std::string& name, float value) const;
    
    /// Set a named uniform of type vec2
    void setVec2(const std::string& name, const glm::vec2& value) const;
    
    /// Set a named uniform of type vec2 as two floats
    void setVec2(const std::string& name, float x, float y) const;

    /// Set a named uniform of type vec3
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    /// Set a named uniform of type vec3 as three floats
    void setVec3(const std::string& name, float x, float y, float z) const;
    
    /// Set a named uniform of type vec4
    void setVec4(const std::string& name, const glm::vec4& value) const;
    
    /// Set a named uniform of type vec4 as four floats
    void setVec4(const std::string& name, float x, float y, float z, float w);
    
    /// Set a named uniform of type mat2
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    
    /// Set a named uniform of type mat3
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    
    /// Set a named uniform of type mat4
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    /// Set a named uniform of the type of vector of int
    void setIntVector(const std::string& name, const std::vector<int>& vector);

  private:
    int createShader(const char* source, GLenum shader_type, const char* description);

    void checkCompileErrors(GLuint shader, std::string type);
    GLuint m_programID;

    bool m_valid;
  };
}

