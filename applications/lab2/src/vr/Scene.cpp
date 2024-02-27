#include <vr/Scene.h>
#include <iostream>
#include <vr/glErrorUtil.h>
#include "lab2/callbacks/RotateCallback.h"

using namespace vr;

Scene::Scene() : m_uniform_numberOfLights(-1)
{
  m_renderToTexture = std::shared_ptr<RenderToTexture>(new RenderToTexture());
  m_camera = std::shared_ptr<Camera>(new Camera);
  m_renderVisitor = new RenderVisitor();
  m_renderVisitor->setCamera(m_camera);
  m_updateVisitor = new UpdateVisitor();
  m_renderVisitor->setRTT(m_renderToTexture);
}

void Scene::setDefaultRootState(Group &g)
{
  auto newRootState = std::shared_ptr<State>(new State("root_state"));
  newRootState->setShader(m_defaultShader);
  newRootState->setEnableLight(std::shared_ptr<bool>(new bool(true)));
  newRootState->setMaterial(std::shared_ptr<vr::Material>(new Material()));
  g.setState(newRootState);
}

bool Scene::initShaders(const std::string& vshader_filename, const std::string& fshader_filename)
{
  auto shader = std::shared_ptr<vr::Shader>(new Shader(vshader_filename, fshader_filename));
  if (!shader->valid())
    return false;
  m_defaultShader = shader;
  return true;
}

std::shared_ptr<Camera> Scene::getCamera() { return m_camera; }
void Scene::setUseShadowMap(bool b) { m_useShadowMap = b; }
bool Scene::getUseShadowMap(void) { return m_useShadowMap; }
void Scene::setUseGroundPlane(bool b) { m_useGroundPlane = b; }
bool Scene::getUseGroundPlane(void) { return m_useGroundPlane; }

RenderVisitor *Scene::getRenderVisitor(void) { return m_renderVisitor; }
std::shared_ptr<Light> Scene::getLight(int i) { return m_sceneLights[i]; }
std::vector<std::shared_ptr<Light>> Scene::getLights() { return m_sceneLights; }
void Scene::addLight(std::shared_ptr<Light> newLight) { m_sceneLights.push_back(newLight); }


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
  new_geo->initShaders(m_defaultShader);
  CHECK_GL_ERROR_LINE_FILE();
  new_geo->upload();
  CHECK_GL_ERROR_LINE_FILE();

  auto defTransform = new Transform("Default_Trans");
  defTransform->addUpdateCallback(new RotateCallback(1, glm::vec3(0,1,0)));
  defTransform->addChild(new_geo);
  defaultScene->addChild(defTransform);

  return defaultScene;
}

void Scene::addGroundPlane()
{  
  auto groundGroup = new Group("Ground_Plane_Group");
  std::vector<glm::vec3> vertices = {
    glm::vec3(-1000.0f, 0.0f, -1000.0f),
    glm::vec3(1000.0f, 0.0f, -1000.0f),
    glm::vec3(1000.0f, 0.0f, 1000.0f),
    glm::vec3(-1000.0f, 0.0f, 1000.0f)
  };

  std::vector<GLshort> indices = {
    0, 1, 2,
    2, 3, 0
  };
  
  std::vector<glm::vec3> normals = {
    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)),
    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))
  };

  std::vector<glm::vec2> texCoords = {
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 1.0f)
  };

  auto groundState = std::shared_ptr<State>(new State("ground_state"));
  auto groundMat = std::shared_ptr<vr::Material>(new Material());
  groundMat->setSpecular(glm::vec4(0.0, 0.0, 0.0, 1.0));
  groundState->setMaterial(groundMat);
  auto groundPlane = buildGeometry("ground_geo", vertices, indices, normals, texCoords);
  groundPlane->setIsGround(true);
  groundPlane->initShaders(m_defaultShader);
  groundPlane->upload();
  groundGroup->addChild(groundPlane);
  groundGroup->setState(groundState);
  m_rootGroup->addChild(groundGroup);
}

void Scene::addLightMatrix(glm::mat4 lm) { m_lightMatrices.push_back(lm); }
void Scene::updateLightMatrix(int idx, BoundingBox box, glm::vec2 nearFar)
{
  m_sceneLights[idx]->calcLightMatrices(box, nearFar);
  m_lightMatrices[idx] = m_sceneLights[idx]->getLightMatrix();
}

void Scene::render()
{
  m_renderVisitor->setLightMatrices(m_lightMatrices);
  m_renderVisitor->setUseShadowMap(m_useShadowMap);

  if(m_useShadowMap) {
    m_renderVisitor->setDepthPass(true);

    for(int i = 0; i < m_sceneLights.size(); i++) {
      if(m_sceneLights[i]->getPosition().w == 0.0) {
        m_renderVisitor->getRTT()->switchToDepthTexture(i);
      } else {
        m_renderVisitor->getRTT()->switchToDepthCubemap(i);
      }
      m_renderVisitor->setCurrentLight(i);
      m_renderVisitor->visit(*m_rootGroup);
    }

    m_renderVisitor->getRTT()->defaultBuffer();
    m_renderVisitor->setDepthPass(false);
  }

  m_renderVisitor->visit(*m_rootGroup);
  m_updateVisitor->visit(*m_rootGroup);
}