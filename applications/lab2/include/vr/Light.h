#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vr/Shader.h>

class Geometry;
namespace vr
{
  /// Simple class that store light properties and apply them to Uniforms
  class Light
  {
  public:

    Light();

    void apply(std::shared_ptr<vr::Shader> shader, size_t idx);
    void setAttenuation(float c, float l, float q);    
    void setPosition(glm::vec4 p) { m_position = p; }
    glm::vec4 getPosition(void) { return m_position; }
    void setAmbient(glm::vec4 a) { m_ambient = a; }
    glm::vec4 getAmbient(void) { return m_ambient; }
    void setDiffuse(glm::vec4 d) { m_diffuse = d; }
    glm::vec4 getDiffuse(void) { return m_diffuse; }
    void setSpecular(glm::vec4 s) { m_specular = s; }
    glm::vec4 getSpecular(void) { return m_specular; }

  private:
    friend class Scene;
    std::shared_ptr<Geometry> m_geo;

    bool enabled;
    glm::vec4 m_ambient;
    glm::vec4 m_position;
    glm::vec4 m_diffuse;
    glm::vec4 m_specular;

    float m_att_constant;
    float m_att_linear;
    float m_att_quadratic;

    void createGeometry();
    std::shared_ptr<Geometry>& getGeometry();
  };
  typedef std::vector<std::shared_ptr<Light> > LightVector;


}