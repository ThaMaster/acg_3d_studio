#include <vr/Application.h>

#include <iostream>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vr/FileSystem.h>

#include <vr/Scene.h>
#include <vr/Loader.h>

#include <glm/gtx/string_cast.hpp>


using namespace vr;

Application::Application(unsigned int width, unsigned height) : 
  m_screenSize(width, height), 
  m_clearColor(1, 1, 1, 1),
  m_lastTime(0)
{
  for(int i = 0; i < 10; i++)
  m_numPressed.push_back(false);
  m_fpsCounter = std::make_shared<FPSCounter>();
  m_fpsCounter->setFontScale(0.5f);
  m_fpsCounter->setColor(glm::vec4(0.2, 1.0, 1.0, 1.0));
}

bool Application::initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename)
{
  m_loadedVShader = vshader_filename;
  m_loadedFShader = fshader_filename;
  m_loadedFilename = model_filename;
  m_sceneRoot = std::shared_ptr<Scene>(new Scene);

  if (!m_sceneRoot->initShaders(vshader_filename, fshader_filename))
    return false;

  getCamera()->setScreenSize(m_screenSize);
  
  std::string ext = vr::FileSystem::getFileExtension(model_filename);

  // Ok lets load this as our own "scene file format"
  if (ext == "xml" || ext == "XML") {
    if (!loadSceneFile(model_filename, m_sceneRoot))
      return false;
  } else {

    auto newRootGroup = new Group("root_group");
    m_sceneRoot->setDefaultRootState(*newRootGroup);
  
    if(model_filename.empty()) {
      newRootGroup->addChild(m_sceneRoot->createDefaultScene());
      m_sceneRoot->setRootGroup(newRootGroup);
    } else {
      auto objNode = load3DModelFile(model_filename, model_filename, m_sceneRoot);
      if (!objNode)
        return false;
      newRootGroup->addChild(objNode);
      m_sceneRoot->setRootGroup(newRootGroup);
    }
  }
  if(m_sceneRoot->getUseGroundPlane())
    m_sceneRoot->addGroundPlane();

  if(m_sceneRoot->getLights().size() == 0) {
    std::shared_ptr<Light> light1 = std::shared_ptr<Light>(new Light);
    light1->setAmbient(glm::vec4(0.1, 0.1, 0.1, 1.0));
    light1->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0));
    light1->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0));
    light1->setPosition(glm::vec4(0.0, 1.0, 1.0, 0.0));
    m_sceneRoot->getRootGroup()->getState()->addLight(light1);
    m_sceneRoot->addLight(light1);
    m_sceneRoot->getRTT()->addDepthMap();
  }

  return 1;
}

void Application::reloadScene()
{
  if(!initResources(m_loadedFilename, m_loadedVShader, m_loadedFShader)) {
    std::cout << "ERROR: Could not initialize resources!" << std::endl;
    exit(1);
  }
  initView();
}

void Application::setClearColor(const glm::f32vec4& clearColor)
{
  m_clearColor = clearColor;
}

void Application::initView()
{
  // Compute a bounding box around the whole scene
  m_bbox = m_sceneRoot->getRootGroup()->calculateBoundingBox(glm::mat4(1));  
  float radius = m_bbox.getRadius();

  // Compute the diagonal and a suitable distance so we can see the whole thing
  float distance = radius * 1.5f;
  glm::vec3 eye = glm::vec3(0, radius, distance);

  glm::vec3 direction = glm::normalize(m_bbox.getCenter() - eye);

  glm::vec4 position;
  position = glm::vec4(eye + glm::vec3(-8, 2, 0), 1);

  // Set the position/direction of the camera
  getCamera()->set(eye, direction, glm::vec3(0.0, 1.0, 0.0));
  getCamera()->setDefaultView(eye, direction, glm::vec3(0.0, 1.0, 0.0));
  getCamera()->setNearFar(glm::vec2(0.1, distance * 20));

  // Compute the default movement speed based on the radius of the scene
  getCamera()->setSpeed(0.7f * radius);

  for(int i = 0; i < m_sceneRoot->getLights().size(); i++) {
    m_sceneRoot->updateLightMatrices(i, m_bbox, getCamera()->getNearFar());
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
}

void Application::render(GLFWwindow* window)
{
  m_sceneRoot->render();
  // m_fpsCounter->render(window);
}

void Application::update(GLFWwindow* window)
{
  render(window);
}

void Application::processInput(GLFWwindow* window)
{
  getCamera()->processInput(window);
  std::shared_ptr<vr::Light> light = m_sceneRoot->getSelectedLight();
  if(light) {
    lightInput(window, light);
    m_sceneRoot->updateLightMatrices(m_sceneRoot->getSelectedLightIdx(), m_bbox, getCamera()->getNearFar());
  }

  quadInput(window);

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    if(!m_spacePressed) {
      m_sceneRoot->setUseShadowMap(!m_sceneRoot->getUseShadowMap());
      m_spacePressed = true;
    }
  }

  if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
    m_spacePressed = false;
}

void Application::lightInput(GLFWwindow* window, std::shared_ptr<vr::Light> light)
{
  float currentTime = (float)glfwGetTime();
  float deltaTime = float(currentTime - m_lastTime);
  float m_speed = 0.1f;
  glm::vec4 deltaPos = glm::vec4(0.0f);
  
  if(light->isEnabled()) {
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) deltaPos.z -= m_speed;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) deltaPos.z += m_speed;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) deltaPos.x -= m_speed;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) deltaPos.x += m_speed;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) deltaPos.y += m_speed;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) deltaPos.y -= m_speed;
    light->setPosition(light->getPosition() + deltaPos);
  }

  if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    if(!m_lightSwitchPressed) {
      m_sceneRoot->selectNextLight();
      m_lightSwitchPressed = true;
    }
  }

  if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
    if(!m_lightTogglePressed) {
      m_sceneRoot->getSelectedLight()->toggleLight();
      m_lightTogglePressed = true;
    }
  }

  if(glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
    m_lightSwitchPressed = false;
  
  if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
    m_lightTogglePressed = false;
}

void Application::quadInput(GLFWwindow* window)
{
  int quadIdx = -1;
  for (int i = 0; i < 10; i++) {
    int key = GLFW_KEY_0 + i;
    bool keyPressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool keyReleased = glfwGetKey(window, key) == GLFW_RELEASE;

    if (keyPressed && !m_numPressed[i]) {
        m_numPressed[i] = true;
        if(i == 0)
          quadIdx = 9;
        else
          quadIdx = i - 1;
    } else if (keyReleased) {
        m_numPressed[i] = false;
    }
  }
  if (quadIdx != -1) {
      m_sceneRoot->toggleQuad(quadIdx);
  }
}

void Application::setScreenSize(unsigned int width, unsigned int height)
{
  getCamera()->setScreenSize(glm::uvec2(width, height));
  glViewport(0, 0, width, height);
}

std::shared_ptr<Camera> Application::getCamera()
{
  return m_sceneRoot->getCamera();
}

Application::~Application()
{
}