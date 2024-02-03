#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Mesh.h"
#include <vector>
#include <vr/Shader.h>

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
    std::shared_ptr<Mesh> m_mesh;

    void createMesh();
    std::shared_ptr<Mesh>& getMesh();
  };
  typedef std::vector<std::shared_ptr<Light> > LightVector;


}