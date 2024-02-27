
#pragma once

#include <memory>
#include <vector>
#include <map>
#include <sstream>

#include <vr/Light.h>
#include <vr/Camera.h>
#include <vr/Shader.h>
#include <vr/BoundingBox.h>

#include "lab2/RenderToTexture.h"
#include "lab2/nodes/Group.h"
#include "lab2/nodes/Geometry.h"
#include "lab2/visitors/RenderVisitor.h"
#include "lab2/visitors/UpdateVisitor.h"

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

    void setDefaultRootState(Group& g);

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

    void addGroundPlane();
    void setUseShadowMap(bool b);
    bool getUseShadowMap(void);
    void setUseGroundPlane(bool b);
    bool getUseGroundPlane(void);

    RenderVisitor *getRenderVisitor(void);
    std::shared_ptr<Light> getLight(int i);
    std::vector<std::shared_ptr<Light>> getLights();
    void addLight(std::shared_ptr<Light> newLight);

    std::vector<std::vector<glm::mat4>> getLightMatrices(void) { return m_lightMatrices; }
    void addLightMatrices(std::vector<glm::mat4> lm);
    void updateLightMatrices(int idx, BoundingBox box, glm::vec2 nearFar);

    std::shared_ptr<RenderToTexture> getRTT(void) { return m_renderToTexture; }

    std::shared_ptr<Shader> getDefaultShader(void) { return m_defaultShader; }

  private:
    std::shared_ptr<RenderToTexture> m_renderToTexture;
    bool m_useShadowMap = true;
    bool m_useGroundPlane = true;
    GLint m_uniform_numberOfLights;
    Group *m_rootGroup;
    RenderVisitor *m_renderVisitor;
    UpdateVisitor *m_updateVisitor;
    std::shared_ptr<Camera> m_camera;
    std::map<std::string, Group*> m_loadedObjects;

    std::shared_ptr<Shader> m_defaultShader;

    std::vector<std::shared_ptr<Light>> m_sceneLights;
    std::vector<std::vector<glm::mat4>> m_lightMatrices;
  };
}