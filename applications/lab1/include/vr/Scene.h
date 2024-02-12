
#pragma once

#include <memory>
#include <vector>
#include <map>
#include <sstream>

#include <vr/Light.h>
#include <vr/Camera.h>
#include <vr/Shader.h>

#include "lab1/nodes/Group.h"
#include "lab1/nodes/Geometry.h"
#include "lab1/visitors/RenderVisitor.h"
#include "lab1/visitors/UpdateVisitor.h"
namespace vr
{
  /**
  Class that holds all mesh objects, lights and a camera
  */
  class Scene
  {
  public:

    /**
    Constructor
    */
    Scene();

    /**
    Initialize the vertex and fragment shader and a program.
    \param vshader_filename
    \param fshader_filename
    \return true if successful.
    */
    bool initShaders(const std::string& vshader_filename, const std::string& fshader_filename);

    /**
    Add a lightsource
    \param light - A new light source<
    */
    void add(std::shared_ptr<Light>& light);

    /**
    Get the camera
    \return The camera
    */
    std::shared_ptr<Camera> getCamera();

    ~Scene();

    /**
    Render the whole scene
    */
    void render();

    void setRootGroup(Group *g)
    {
      m_rootGroup = g;
    }

    Group* getRootGroup()
    {
      return m_rootGroup;
    }

    bool objectExists(std::string filePath)
    {
      return m_loadedObjects.find(filePath) != m_loadedObjects.end();
    }

    Group* getObject(std::string filePath)
    {
      return m_loadedObjects[filePath];
    }

    void addObject(std::string filePath, Group* geometry)
    {
      m_loadedObjects.insert(std::make_pair(filePath, geometry));
    }

    Group* createDefaultScene();
    Geometry* buildGeometry(std::string geo_name, std::vector<glm::vec3> vertices, std::vector<GLshort> indices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords);
    void setUseDefaultLight(bool b) { m_useDefaultLight = b; }
    bool getUseDefaultLight(void) { return m_useDefaultLight; }

  private:
    bool m_useDefaultLight = true;
    GLint m_uniform_numberOfLights;
    Group *m_rootGroup;
    RenderVisitor *m_renderVisitor;
    UpdateVisitor *m_updateVisitor;
    std::shared_ptr<Camera> m_camera;
    std::map<std::string, Group*> m_loadedObjects;
  };
}