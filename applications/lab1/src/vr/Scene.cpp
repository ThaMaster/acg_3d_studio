#include <vr/Scene.h>
#include <iostream>
#include <vr/glErrorUtil.h>

using namespace vr;

Scene::Scene() : m_uniform_numberOfLights(-1)
{
  m_camera = std::shared_ptr<Camera>(new Camera);
  setRootGroup(new Group("root"));
  getRootGroup()->setState(new State("root_state"));
  m_renderVisitor = new RenderVisitor();
  m_updateVisitor = new UpdateVisitor();
}

bool Scene::initShaders(const std::string& vshader_filename, const std::string& fshader_filename)
{
  m_shader = std::make_shared<vr::Shader>(vshader_filename, fshader_filename);
  m_renderVisitor->setShader(m_shader);
  if (!m_shader->valid())
    return false;

  return true;
}

void Scene::add(std::shared_ptr<Light>& light)
{
  m_lights.push_back(light);
  std::shared_ptr<Node> node = std::shared_ptr<Node>(new Node());

  node->add(light->m_mesh);
  node->add(light->getMesh());

  // Also add the mesh-node
  add(node);
}

const LightVector& Scene::getLights()
{
  return m_lights;
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
  m_renderVisitor->getShader()->use();
}

void Scene::add(std::shared_ptr<Node>& node)
{
  m_nodes.push_back(node);

  for (auto m : node->getMeshes())
  {
    m->initShaders(m_shader);
    m->upload();
  }
}

void Scene::addObj(std::shared_ptr<Geometry>& obj)
{
  m_objects.push_back(obj);
}

void Scene::resetTransform()
{
  for (auto n : m_nodes)
    n->resetTransform();
}

const NodeVector& Scene::getNodes()
{
  return m_nodes;
}

std::shared_ptr<Node> Scene::getNode(size_t i)
{
  return m_nodes[i];
}

BoundingBox Scene::calculateBoundingBox()
{
  BoundingBox box;
  for (auto n : m_nodes)
    box.expand(n->calculateBoundingBox());

  return box;
}

void Scene::render()
{
  CHECK_GL_ERROR_LINE_FILE();
  useProgram();

  CHECK_GL_ERROR_LINE_FILE();

  // Update number of lights
  m_shader->setInt("numberOfLights", (GLint)m_lights.size());

  // Apply lightsources
  size_t i = 0;
  for (auto l : m_lights)
  {
    l->apply(m_shader, i);
  }
  m_renderVisitor->setShader(m_shader);
  m_renderVisitor->visit(*m_rootGroup);
  m_updateVisitor->visit(*m_rootGroup);
  
  
}