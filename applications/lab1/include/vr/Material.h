#pragma once

// Important: Import glad as first gl header
#include <glad/glad.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <vr/Texture.h>
#include <vr/Shader.h>
namespace vr
{

  /// Simple class for storing material properties
  class Material
  {
  private:

    glm::vec4 m_ambient, m_diffuse, m_specular;

    GLfloat m_shininess;

    typedef std::vector<std::shared_ptr<vr::Texture> > TextureVector;
    TextureVector m_textures;

  public:

    Material();

    glm::vec4 getAmbient() const;
    glm::vec4 getSpecular() const;
    glm::vec4 getDiffuse() const;

    void setAmbient(const glm::vec4& color);
    void setSpecular(const glm::vec4& color);
    void setDiffuse(const glm::vec4& color);
    void setShininess(float s);

    /// Set a \p texture to the specified \p unit
    void setTexture(std::shared_ptr<vr::Texture> texture, unsigned int unit);

    /// Apply the material to the choosen shader
    void apply(std::shared_ptr<vr::Shader> shader);
  };

  typedef std::vector<std::shared_ptr<Material> > MaterialVector;

}