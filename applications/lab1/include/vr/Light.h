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
    bool enabled;
    glm::vec4 position;
    glm::vec4 diffuse;
    glm::vec4 specular;

    Light();

    void apply(std::shared_ptr<vr::Shader> shader, size_t idx);
    
  private:
    friend class Scene;
    std::shared_ptr<Geometry> m_geo;

    void createGeometry();
    std::shared_ptr<Geometry>& getGeometry();
  };
  typedef std::vector<std::shared_ptr<Light> > LightVector;


}