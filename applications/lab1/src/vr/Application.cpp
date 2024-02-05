#include <vr/Application.h>

#include <iostream>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vr/FileSystem.h>

#include <vr/Scene.h>
#include <vr/Loader.h>

using namespace vr;

Application::Application(unsigned int width, unsigned height) : 
  m_screenSize(width, height), 
  m_clearColor(1, 1, 1, 1)
{
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
  if (ext == "xml" || ext == "XML")
  {
    if (!loadSceneFile(model_filename, m_sceneRoot))
      return false;
  }
  else
  {
    auto objNode = load3DModelFile(model_filename, model_filename);
    if (!objNode)
      return false;
    m_sceneRoot->getRootGroup()->addChild(objNode);
  }

  std::shared_ptr<Light> light1 = std::shared_ptr<Light>(new Light);
  light1->diffuse = glm::vec4(1, 1, 1, 1);
  light1->specular = glm::vec4(1, 1, 1, 1);
  light1->position = glm::vec4(0.0, -2.0, 2.0, 0.0);

  m_sceneRoot->add(light1);

  return 1;
}

void Application::reloadScene()
{
  initResources(m_loadedFilename, m_loadedVShader, m_loadedFShader);
  initView();
}

void Application::setClearColor(const glm::f32vec4& clearColor)
{
  m_clearColor = clearColor;
}

void Application::initView()
{
  // Compute a bounding box around the whole scene
  BoundingBox box = m_sceneRoot->getRootGroup()->calculateBoundingBox();

  float radius = box.getRadius();

  // Compute the diagonal and a suitable distance so we can see the whole thing
  float distance = radius * 1.5f;
  glm::vec3 eye = glm::vec3(0, radius, distance);

  glm::vec3 direction = glm::normalize(box.getCenter() - eye);

  std::shared_ptr<Light> light = m_sceneRoot->getRootGroup()->getState()->getLights().front();
  glm::vec4 position;
  position = glm::vec4(eye + glm::vec3(-8, 2, 0), 1);
  light->position = position;

  m_sceneRoot->resetTransform();

  // Set the position/direction of the camera
  getCamera()->set(eye, direction, glm::vec3(0.0, 1.0, 0.0));
  getCamera()->setDefaultView(eye, direction, glm::vec3(0.0, 1.0, 0.0));
  getCamera()->setNearFar(glm::vec2(0.1, distance * 20));

  // Compute the default movement speed based on the radius of the scene
  getCamera()->setSpeed(0.7f * radius);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
}

void Application::render(GLFWwindow* window)
{
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_sceneRoot->applyCamera();
  m_sceneRoot->render();
  m_fpsCounter->render(window);
}

void Application::update(GLFWwindow* window)
{
  m_sceneRoot->useProgram();

  render(window);
}

void Application::processInput(GLFWwindow* window)
{
  getCamera()->processInput(window);
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