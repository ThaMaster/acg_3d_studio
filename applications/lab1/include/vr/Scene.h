
#pragma once

#include <memory>
#include <vector>
#include <map>
#include <sstream>

#include <vr/Light.h>
#include <vr/Camera.h>
#include <vr/Shader.h>

#include "lab1/nodes/Group.h"
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
    Apply the camera
    */
    void applyCamera();

    /**
    Use the shader program
    */
    void useProgram();
    
    /**
    Reset all nodes transform to its initial value
    */
    void resetTransform();

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

    Group* createDefaultScene();

  private:
    GLint m_uniform_numberOfLights;
    Group *m_rootGroup;
    RenderVisitor *m_renderVisitor;
    UpdateVisitor *m_updateVisitor;
    std::shared_ptr<Camera> m_camera;
  };
}