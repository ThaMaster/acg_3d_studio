#include <vr/Scene.h>
#include <iostream>
#include <vr/glErrorUtil.h>

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
  m_shader = std::make_shared<vr::Shader>(vshader_filename, fshader_filename);
  getRootGroup()->getState()->setShader(m_shader);
  if (!m_shader->valid())
    return false;

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

Scene::~Scene()
{

}

void Scene::applyCamera()
{
  m_camera->apply(m_shader);
}

void Scene::useProgram()
{
  getRootGroup()->getState()->getShader()->use();
}

void Scene::resetTransform()
{
}

void Scene::render()
{
  m_renderVisitor->visit(*m_rootGroup);
  m_updateVisitor->visit(*m_rootGroup);
}