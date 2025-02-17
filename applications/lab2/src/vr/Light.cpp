#include <vr/Light.h>
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lab2/nodes/Geometry.h"

#include <glm/gtx/string_cast.hpp>

using namespace vr;

Light::Light() : enabled(true)
{
  createGeometry();
  m_ambient = glm::vec4(0.2, 0.2, 0.2, 1.0);
  m_att_constant = 1.0;
  m_att_linear = 0.09;
  m_att_quadratic = 0.032;
  
  for(int i = 0; i < 6; i++)
    m_lightMatrices.push_back(glm::mat4(1.0f));
}

void Light::setPosition(glm::vec4 p) { m_position = p; }
glm::vec4 Light::getPosition(void) { return m_position; }
void Light::setAmbient(glm::vec4 a) { m_ambient = a; }
glm::vec4 Light::getAmbient(void) { return m_ambient; }
void Light::setDiffuse(glm::vec4 d) { m_diffuse = d; }
glm::vec4 Light::getDiffuse(void) { return m_diffuse; }
void Light::setSpecular(glm::vec4 s) { m_specular = s; }
glm::vec4 Light::getSpecular(void) { return m_specular; }

std::vector<glm::mat4> Light::getLightMatrices(void) { return m_lightMatrices; }

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

  std::string uniform_name = prefix + "enabled";

  shader->setInt(uniform_name, enabled);
  shader->setVec4(prefix + "ambient", this->m_ambient);
  shader->setVec4(prefix + "diffuse", this->m_diffuse);
  shader->setVec4(prefix + "specular", this->m_specular);
  shader->setVec4(prefix + "position", this->m_position);
  
  if(m_position.w == 0.0) {
    shader->setMat4(prefix + "lightMatrices[0]", m_lightMatrices[0]);
  } else {
    shader->setFloat(prefix + "constant", this->m_att_constant);
    shader->setFloat(prefix + "linear", this->m_att_linear);
    shader->setFloat(prefix + "quadratic", this->m_att_quadratic);

    for(int i = 0; i < 6; i++) {
      std::stringstream matStr;
      matStr << "lightMatrices[" << i << "]";
      std::string matPref = matStr.str();
      shader->setMat4(prefix + matPref, this->m_lightMatrices[i]);
    }
  }
}

void Light::calcLightMatrices(vr::BoundingBox box, glm::vec2 nearFar)
{
  std::vector<glm::mat4> newLightMatrices;
  glm::mat4 depthViewMatrix = glm::mat4(1.0f);
  glm::mat4 depthProjectionMatrix = glm::mat4(1.0f);
  auto radius = box.getRadius();
  if(m_position.w == 0.0) {
    depthViewMatrix = glm::lookAt(glm::vec3(m_position) + box.getCenter(), box.getCenter(), glm::vec3(0,1,0));
    depthProjectionMatrix = glm::ortho<float>(-radius*1.5, radius*1.5, -radius*1.5, radius*1.5, -radius, nearFar.y);
    newLightMatrices.push_back(depthProjectionMatrix * depthViewMatrix);
  } else {
    depthProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, nearFar.x, nearFar.y);
    newLightMatrices.push_back(depthProjectionMatrix * glm::lookAt(glm::vec3(m_position), glm::vec3(m_position) + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    newLightMatrices.push_back(depthProjectionMatrix * glm::lookAt(glm::vec3(m_position), glm::vec3(m_position) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    newLightMatrices.push_back(depthProjectionMatrix * glm::lookAt(glm::vec3(m_position), glm::vec3(m_position) + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    newLightMatrices.push_back(depthProjectionMatrix * glm::lookAt(glm::vec3(m_position), glm::vec3(m_position) + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    newLightMatrices.push_back(depthProjectionMatrix * glm::lookAt(glm::vec3(m_position), glm::vec3(m_position) + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
    newLightMatrices.push_back(depthProjectionMatrix * glm::lookAt(glm::vec3(m_position), glm::vec3(m_position) + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
  }
  m_lightMatrices = newLightMatrices;
}
