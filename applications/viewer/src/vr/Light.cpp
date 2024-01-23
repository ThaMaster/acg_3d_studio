#include <vr/Light.h>
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace vr;

Light::Light() : enabled(true)
{
  createMesh();
}


void Light::createMesh()
{
  m_mesh = std::shared_ptr<Mesh>(new Mesh);

  float size = 0.1f;
  glm::vec3 light_position = glm::vec3(0.0, 1.0, 2.0);
  m_mesh->vertices.push_back(glm::vec4(-size, -size, -size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(size, -size, -size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(size, size, -size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(-size, size, -size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(-size, -size, size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(size, -size, size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(size, size, size, 0.0));
  m_mesh->vertices.push_back(glm::vec4(-size, size, size, 0.0));
  m_mesh->object2world = glm::translate(glm::mat4(1), glm::vec3(this->position));
  m_mesh->name = "light";
}

std::shared_ptr<Mesh>& Light::getMesh() 
{ 
  return m_mesh; 
}

void Light::apply(std::shared_ptr<vr::Shader> shader, size_t idx)
{
  int i = 0;

  // Update light position
  m_mesh->object2world = glm::translate(glm::mat4(1), glm::vec3(this->position));

  std::stringstream str;
  str << "lights[" << idx << "].";
  std::string prefix = str.str();

  GLint loc = -1;
  std::string uniform_name;

  uniform_name = prefix + "enabled";

  shader->setInt(uniform_name, enabled);
  shader->setVec4(prefix + "diffuse", this->diffuse);
  shader->setVec4(prefix + "specular", this->specular);
  shader->setVec4(prefix + "position", this->position);
}
