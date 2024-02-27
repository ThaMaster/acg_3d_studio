#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vr/Shader.h>

#include "vr/BoundingBox.h"
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

    void setPosition(glm::vec4 p);
    glm::vec4 getPosition(void);

    void setAmbient(glm::vec4 a);
    glm::vec4 getAmbient(void);

    void setDiffuse(glm::vec4 d);
    glm::vec4 getDiffuse(void);
    
    void setSpecular(glm::vec4 s);
    glm::vec4 getSpecular(void);

    void calcLightMatrices(vr::BoundingBox box, glm::vec2 nearFar);
    std::vector<glm::mat4> getLightMatrices(void);

  private:
    friend class Scene;
    std::shared_ptr<Geometry> m_geo;

    bool enabled;
    glm::vec4 m_ambient;
    glm::vec4 m_position;
    glm::vec4 m_diffuse;
    glm::vec4 m_specular;

    std::vector<glm::mat4> m_lightMatrices;

    GLuint m_shadowMap = 0;

    float m_att_constant;
    float m_att_linear;
    float m_att_quadratic;

    float m_lastTime;

    void createGeometry();
    std::shared_ptr<Geometry>& getGeometry();
  };

  typedef std::vector<std::shared_ptr<Light>> LightVector;
}