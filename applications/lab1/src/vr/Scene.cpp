#include <vr/Scene.h>
#include <iostream>
#include <vr/glErrorUtil.h>
#include "lab1/callbacks/RotateCallback.h"

using namespace vr;

Scene::Scene() : m_uniform_numberOfLights(-1)
{
  m_camera = std::shared_ptr<Camera>(new Camera);
  setRootGroup(new Group("root"));
  getRootGroup()->setState(std::shared_ptr<State>(new State("root_state")));
  m_renderVisitor = new RenderVisitor();
  m_updateVisitor = new UpdateVisitor();
}

bool Scene::initShaders(const std::string& vshader_filename, const std::string& fshader_filename)
{
  auto shader = std::make_shared<vr::Shader>(vshader_filename, fshader_filename);
  if (!shader->valid())
    return false;

  getRootGroup()->getState()->setShader(shader);
  return true;
}

void Scene::add(std::shared_ptr<Light>& light)
{
  getRootGroup()->getState()->addLight(light);
}

std::shared_ptr<Camera> Scene::getCamera()
{
  return m_camera;
}

Scene::~Scene() {}

void Scene::applyCamera()
{
  m_camera->apply(getRootGroup()->getState()->getShader());
}

void Scene::useProgram()
{
  getRootGroup()->getState()->getShader()->use();
}

void Scene::resetTransform()
{
}

Geometry* Scene::buildGeometry(std::string geo_name, std::vector<glm::vec3> vertices, std::vector<GLshort> indices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords)
{
  auto geometry = new Geometry(geo_name);

  for(auto v : vertices)
    geometry->vertices.push_back(glm::vec4(v.x, v.y, v.z, 1));

  for(auto i : indices)
    geometry->elements.push_back(i);
  
  for(auto n : normals)
    geometry->normals.push_back(n);

  for(auto t : texCoords)
    geometry->texCoords.push_back(t);

  return geometry;
}

Group* Scene::createDefaultScene()
{
  auto defaultScene = new Group("DefaultSceneRoot");
  std::vector<glm::vec3> vertices = {
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f)
  };

  std::vector<GLshort> indices = {
    0, 1, 2, 2, 3, 0,
    1, 5, 6, 6, 2, 1,
    7, 6, 5, 5, 4, 7,
    4, 0, 3, 3, 7, 4,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3
  };

  // Make the normals point outwards from the corners.
  std::vector<glm::vec3> normals = {
    glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)),
    glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f)),
    glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f)),
    glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f)),
    glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f)),
    glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)),
    glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)),
    glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f))
  };

  std::vector<glm::vec2> texCoords = {
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f), 
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(0.0f, 0.0f), 
    glm::vec2(1.0f, 0.0f), 
    glm::vec2(1.0f, 1.0f), 
    glm::vec2(0.0f, 1.0f) 
  };


  auto defaultMat = std::shared_ptr<Material>(new Material());
  auto materialState = std::shared_ptr<State>(new State("def_mat_state"));
  materialState->setMaterial(defaultMat);
  auto new_geo = buildGeometry("Default_Geo", vertices, indices, normals, texCoords);
  new_geo->setState(materialState);

  auto defTransform = new Transform("Default_Trans");
  defTransform->addUpdateCallback(new RotateCallback(1, glm::vec3(0,1,0)));
  defTransform->addChild(new_geo);
  defaultScene->addChild(defTransform);

  return defaultScene;
}

void Scene::render()
{
  m_renderVisitor->visit(*m_rootGroup);
  m_updateVisitor->visit(*m_rootGroup);
}