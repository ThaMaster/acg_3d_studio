#include <vr/Light.h>
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lab1/nodes/Geometry.h"

using namespace vr;

Light::Light() : enabled(true)
{
  createGeometry();
  m_att_constant = 1.0;
  m_att_linear = 0.09;
  m_att_quadratic = 0.032;
}

void Light::createGeometry()
{
  m_geo = std::shared_ptr<Geometry>(new Geometry("light"));

  float size = 0.1f;
  glm::vec3 light_position = glm::vec3(0.0, -3.0, -7.0);
  m_geo->vertices.push_back(glm::vec4(-size, -size, -size, 0.0));
  m_geo->vertices.push_back(glm::vec4(size, -size, -size, 0.0));
  m_geo->vertices.push_back(glm::vec4(size, size, -size, 0.0));
  m_geo->vertices.push_back(glm::vec4(-size, size, -size, 0.0));
  m_geo->vertices.push_back(glm::vec4(-size, -size, size, 0.0));
  m_geo->vertices.push_back(glm::vec4(size, -size, size, 0.0));
  m_geo->vertices.push_back(glm::vec4(size, size, size, 0.0));
  m_geo->vertices.push_back(glm::vec4(-size, size, size, 0.0));
  glm::mat4 o2w = glm::translate(glm::mat4(1), glm::vec3(this->m_position));
  m_geo->setObject2WorldMat(o2w);
}

std::shared_ptr<Geometry>& Light::getGeometry() 
{ 
  return m_geo; 
}

void Light::setAttenuation(float c, float l, float q)
{
  m_att_constant = c;
  m_att_linear = l;
  m_att_quadratic = q;
}

void Light::apply(std::shared_ptr<vr::Shader> shader, size_t idx)
{
  int i = 0;

  // Update light position
  glm::mat4 o2w = glm::translate(glm::mat4(1), glm::vec3(this->m_position));
  m_geo->setObject2WorldMat(o2w);

  std::stringstream str;
  str << "lights[" << idx << "].";
  std::string prefix = str.str();

  GLint loc = -1;
  std::string uniform_name;

  uniform_name = prefix + "enabled";

  shader->setInt(uniform_name, enabled);
  shader->setVec4(prefix + "ambient", this->m_ambient);
  shader->setVec4(prefix + "diffuse", this->m_diffuse);
  shader->setVec4(prefix + "specular", this->m_specular);
  shader->setVec4(prefix + "position", this->m_position);
  if(m_position.w == 1) {
    shader->setFloat(prefix + "constant", this->m_att_constant);
    shader->setFloat(prefix + "linear", this->m_att_linear);
    shader->setFloat(prefix + "quadratic", this->m_att_quadratic);
  } else {
  }

}
