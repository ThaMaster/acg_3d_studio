#include <vr/Scene.h>
#include <iostream>
#include <vr/glErrorUtil.h>
#include "lab3/callbacks/RotateCallback.h"

using namespace vr;

Scene::Scene() : m_uniform_numberOfLights(-1)
{
  m_renderToTexture = std::shared_ptr<RenderToTexture>(new RenderToTexture());
  m_camera = std::shared_ptr<Camera>(new Camera);
  m_renderVisitor = new RenderVisitor();
  m_renderVisitor->setCamera(m_camera);
  m_updateVisitor = new UpdateVisitor();
  m_renderVisitor->setRTT(m_renderToTexture);
  addDebugQuads();
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
  auto lShader = std::shared_ptr<vr::Shader>(new Shader("shaders/deferredShader.vs", "shaders/deferredShader.fs"));
  if (!lShader->valid())
    return false;
  m_defaultShader = shader;
  
  m_mainQuad = createMainQuad();
  m_mainQuad->setQuadShader(lShader);
  m_mainQuad->initShaders();
  m_mainQuad->uploadQuad();

  return true;
}

std::shared_ptr<Camera> Scene::getCamera() { return m_camera; }
void Scene::setUseShadowMap(bool b) { m_useShadowMap = b; }
bool Scene::getUseShadowMap() { return m_useShadowMap; }
void Scene::setUseGroundPlane(bool b) { m_useGroundPlane = b; }
bool Scene::getUseGroundPlane() { return m_useGroundPlane; }
void Scene::setUseBloom(bool b) { m_useBloom = b; }
bool Scene::getUseBloom() { return m_useBloom; }
void Scene::setUseDOF(bool b) { m_useDOF = b; }
bool Scene::getUseDOF() { return m_useDOF; }

RenderVisitor *Scene::getRenderVisitor(void) { return m_renderVisitor; }
std::shared_ptr<Light> Scene::getSelectedLight() { return m_sceneLights[m_selectedLight]; }
std::vector<std::shared_ptr<Light>> Scene::getLights() { return m_sceneLights; }
void Scene::addLight(std::shared_ptr<Light> newLight) { m_sceneLights.push_back(newLight); }


Scene::~Scene() {}

Geometry* Scene::buildGeometry(std::string geo_name, std::vector<glm::vec3> vertices, std::vector<GLushort> indices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords)
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

void Scene::toggleQuad(int quadIdx)
{
  if(!(quadIdx >= m_quads.size() || quadIdx < 0))
  {
    if(m_quadsToRender[quadIdx] == 1)
      m_quadsToRender[quadIdx] = 0;
    else
      m_quadsToRender[quadIdx] = 1;
  }
}

Quad* Scene::buildQuad(std::vector<glm::vec4> vertices, std::vector<GLushort> indices, std::vector<glm::vec2> texCoords)
{
  auto newQuad = new Quad();
  newQuad->setVertices(vertices);
  newQuad->setElements(indices);
  newQuad->setTexCoords(texCoords);
  return newQuad;
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

  std::vector<GLushort> indices = {
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

  std::vector<GLushort> indices = {
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

void Scene::updateLightMatrices(int idx, BoundingBox box, glm::vec2 nearFar)
{
  m_sceneLights[idx]->calcLightMatrices(box, nearFar);
}

std::shared_ptr<Quad> Scene::createMainQuad(void)
{
  std::vector<glm::vec4> vertices = {
    glm::vec4(-1.0f, -1.0f, 0.0f, 1.0), // Bottom Left
    glm::vec4(1.0f, -1.0f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0),   // Top Right
    glm::vec4(-1.0f, 1.0f, 0.0f, 1.0)   // Top Left
  };

  std::vector<GLushort> indices = {
      0, 1, 2, // First Triangle
      2, 3, 0  // Second Triangle
  };

  std::vector<glm::vec2> texCoords = {
      glm::vec2(0.0f, 0.0f), // Bottom Left
      glm::vec2(1.0f, 0.0f), // Bottom Right
      glm::vec2(1.0f, 1.0f), // Top Right
      glm::vec2(0.0f, 1.0f)  // Top Left
  };
  auto newQuad = std::shared_ptr<Quad>(buildQuad(vertices, indices, texCoords));
  return newQuad;
}

void Scene::addDebugQuads(void)
{
  std::vector<std::vector<glm::vec4>> debugQuads;

  // Debug Quad #1 (Positions)
  debugQuads.push_back({
    glm::vec4(-1.0f, 0.5f, 0.0f, 1.0), // Bottom Left
    glm::vec4(-0.5f, 0.5f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(-0.5f, 1.0f, 0.0f, 1.0),   // Top Right
    glm::vec4(-1.0f, 1.0f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #2 (Normals)
  debugQuads.push_back({
    glm::vec4(-0.5f, 0.5f, 0.0f, 1.0), // Bottom Left
    glm::vec4(0.0f, 0.5f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0),   // Top Right
    glm::vec4(-0.5f, 1.0f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #3 (Diffuse Colors)
  debugQuads.push_back({
    glm::vec4(0.0f, 0.5f, 0.0f, 1.0), // Bottom Left
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(0.5f, 1.0f, 0.0f, 1.0),   // Top Right
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #4 (Specular Colors)
  debugQuads.push_back({
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0), // Bottom Left
    glm::vec4(1.0f, 0.5f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0),   // Top Right
    glm::vec4(0.5f, 1.0f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #5 (Lights depthmap)
  debugQuads.push_back({
    glm::vec4(-1.0f, -1.0f, 0.0f, 1.0), // Bottom Left
    glm::vec4(-0.5f, -1.0f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0),   // Top Right
    glm::vec4(-1.0f, -0.5f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #6
  debugQuads.push_back({
    glm::vec4(-0.5f, -1.0f, 0.0f, 1.0), // Bottom Left
    glm::vec4(0.0f, -1.0f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(0.0f, -0.5f, 0.0f, 1.0),   // Top Right
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #7
  debugQuads.push_back({
    glm::vec4(0.0f, -1.0f, 0.0f, 1.0), // Bottom Left
    glm::vec4(0.5f, -1.0f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(0.5f, -0.5f, 0.0f, 1.0),   // Top Right
    glm::vec4(0.0f, -0.5f, 0.0f, 1.0)   // Top Left
  });

  // Debug Quad #8
  debugQuads.push_back({
    glm::vec4(0.5f, -1.0f, 0.0f, 1.0), // Bottom Left
    glm::vec4(1.0f, -1.0f, 0.0f, 1.0),  // Bottom Right
    glm::vec4(1.0f, -0.5f, 0.0f, 1.0),   // Top Right
    glm::vec4(0.5f, -0.5f, 0.0f, 1.0)   // Top Left
  });

  std::vector<GLushort> indices = {
      0, 1, 2, // First Triangle
      2, 3, 0  // Second Triangle
  };

  std::vector<glm::vec2> texCoords = {
      glm::vec2(0.0f, 0.0f), // Bottom Left
      glm::vec2(1.0f, 0.0f), // Bottom Right
      glm::vec2(1.0f, 1.0f), // Top Right
      glm::vec2(0.0f, 1.0f)  // Top Left
  };

  for(int i = 0; i < debugQuads.size(); i++) {
    auto newQuad = std::shared_ptr<Quad>(buildQuad(debugQuads[i], indices, texCoords));
    newQuad->initShaders();
    newQuad->uploadQuad();
    m_quads.push_back(newQuad);
    m_quadsToRender.push_back(0);
  }
}

int Scene::getNumQuadsToRender(void)
{
  int nrQuadsToRender = 0;
  for(auto i : m_quadsToRender)
    if(i == 1) nrQuadsToRender++;
  
  return nrQuadsToRender;
}

void Scene::render()
{
  glEnable(GL_DEPTH_TEST);
  
  // 1st Pass: THE GEOMETRY PASS

  m_renderVisitor->setGBufferPass(true);
  m_renderVisitor->getRTT()->bindGBuffer();
  m_renderVisitor->visit(*m_rootGroup);
  m_renderVisitor->getRTT()->defaultBuffer();
  m_renderVisitor->setGBufferPass(false);

  // 2nd Pass: THE LIGHTNING PASS

  m_renderVisitor->setUseShadowMap(m_useShadowMap);
  if(m_useShadowMap) {
    m_renderVisitor->setDepthPass(true);
    for(int i = 0; i < m_sceneLights.size(); i++) {
      if(m_sceneLights[i]->getPosition().w == 0.0) {
        m_renderVisitor->getRTT()->switchToDepthTexture(i);
      } else {
        m_renderVisitor->getRTT()->switchToDepthCubeMap(i);
      }
      m_renderVisitor->setCurrentLight(m_sceneLights[i]);
      m_renderVisitor->visit(*m_rootGroup);
    }
    m_renderVisitor->getRTT()->defaultBuffer();
    m_renderVisitor->setDepthPass(false);
  }
  
  glDisable(GL_DEPTH_TEST);

  // 3rd Pass: THE QUAD PASS

  // Render Scnee into framebuffer
  m_renderVisitor->getRTT()->bindFB();
  renderMainQuad();

  // Blur bright framgents with two-pass Gaussian Blur
  //applyBlur();
  // Render color buffer to 2D quad to the default buffer, blending the two images.
  m_renderVisitor->getRTT()->defaultBuffer();

  renderFinalImage();

  renderDebugQuads();

  m_updateVisitor->visit(*m_rootGroup);
}

void Scene::renderMainQuad()
{
  m_mainQuad->getQuadShader()->use();
  m_rootGroup->getState()->applyLights(m_mainQuad->getQuadShader());
  m_camera->applyPos(m_mainQuad->getQuadShader());
  m_renderVisitor->getRTT()->applyGAttribs(m_mainQuad->getQuadShader());

  m_mainQuad->getQuadShader()->setBool("useShadowMap", m_useShadowMap);
  m_mainQuad->getQuadShader()->setFloat("far_plane", getCamera()->getNearFar().y);

  if(m_useShadowMap)
      m_renderVisitor->getRTT()->applyDepthMaps(m_mainQuad->getQuadShader());
  m_mainQuad->drawQuad();
}

void Scene::applyBlur(void)
{
  m_renderVisitor->getRTT()->getBlurShader()->use();
}

void Scene::renderFinalImage()
{
  m_renderVisitor->getRTT()->useBloomShader(m_useBloom);
  m_mainQuad->drawQuad();
}

void Scene::renderDebugQuads()
{
  if(m_quadsToRender[0] == 1) 
  {
    m_quads[0]->getQuadShader()->use();
    m_renderVisitor->getRTT()->applyPositionTexture(m_quads[0]->getQuadShader());
    m_quads[0]->drawQuad();
  }

  if(m_quadsToRender[1] == 1) 
  {
    m_quads[1]->getQuadShader()->use();
    m_renderVisitor->getRTT()->applyNormalTexture(m_quads[1]->getQuadShader());
    m_quads[1]->drawQuad();
  }
  
  if(m_quadsToRender[2] == 1) 
  {
    m_quads[2]->getQuadShader()->use();
    m_renderVisitor->getRTT()->applyDiffuseTexture(m_quads[2]->getQuadShader());
    m_quads[2]->drawQuad();
  }

  if(m_quadsToRender[3] == 1)
  {
    m_quads[3]->getQuadShader()->use();
    m_renderVisitor->getRTT()->applySpecularTexture(m_quads[3]->getQuadShader());
    m_quads[3]->drawQuad();
  }

  if(m_quadsToRender[4] == 1)
  {
    m_quads[4]->getQuadShader()->use();
    m_renderVisitor->getRTT()->applyLightDepth(m_quads[4]->getQuadShader(), m_selectedLight, 
      m_sceneLights[m_selectedLight]->getPosition(), m_camera->getNearFar().y);
    m_quads[4]->drawQuad();
  }

  if(m_quadsToRender[5] == 1)
  {
    m_quads[5]->getQuadShader()->use();
    m_renderVisitor->getRTT()->applyBloomBuffer(m_quads[5]->getQuadShader());
    m_quads[5]->drawQuad();
  }

  if(m_quadsToRender[6] == 1)
  {
    m_quads[6]->getQuadShader()->use();
    m_quads[6]->drawQuad();
  }

  if(m_quadsToRender[7] == 1)
  {
    m_quads[7]->getQuadShader()->use();
    m_quads[7]->drawQuad();
  }
}

int Scene::getSelectedLightIdx(void)
{
  return m_selectedLight;
}

void Scene::selectNextLight(void)
{ 
  m_selectedLight++;
  if(m_selectedLight == m_sceneLights.size())
    m_selectedLight = 0;
}


