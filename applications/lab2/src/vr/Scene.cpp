#include <vr/Scene.h>
#include <iostream>
#include <vr/glErrorUtil.h>
#include "lab2/callbacks/RotateCallback.h"

using namespace vr;

Scene::Scene() : m_uniform_numberOfLights(-1)
{
  m_renderToTexture = std::shared_ptr<RenderToTexture>(new RenderToTexture());
  m_camera = std::shared_ptr<Camera>(new Camera);
  setRootGroup(new Group("root"));
  getRootGroup()->setState(std::shared_ptr<State>(new State("root_state")));
  getRootGroup()->getState()->setEnableLight(std::shared_ptr<bool>(new bool(true)));
  getRootGroup()->getState()->setMaterial(std::shared_ptr<vr::Material>(new Material()));
  m_renderVisitor = new RenderVisitor();
  m_renderVisitor->setCamera(m_camera);
  m_updateVisitor = new UpdateVisitor();
  m_renderToTexture->createRenderTarget();
  m_renderVisitor->setRTT(m_renderToTexture);
}

bool Scene::initShaders(const std::string& vshader_filename, const std::string& fshader_filename)
{
  auto shader = std::shared_ptr<vr::Shader>(new Shader(vshader_filename, fshader_filename));
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
  new_geo->initShaders(m_rootGroup->getState()->getShader());
  new_geo->upload();

  auto defTransform = new Transform("Default_Trans");
  defTransform->addUpdateCallback(new RotateCallback(1, glm::vec3(0,1,0)));
  defTransform->addChild(new_geo);
  defaultScene->addChild(defTransform);

  return defaultScene;
}

void Scene::render()
{
  m_renderVisitor->setDepthPass(true);
  m_renderVisitor->getRTT()->getDepthShader()->use();
  m_renderVisitor->getRTT()->switchToFramebuffer();
  m_renderVisitor->visit(*m_rootGroup);
  m_renderVisitor->getRTT()->defaultBuffer();
  m_renderVisitor->setDepthPass(false);
  m_renderVisitor->visit(*m_rootGroup);
  m_updateVisitor->visit(*m_rootGroup);
}