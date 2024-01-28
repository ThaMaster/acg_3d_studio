
#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include "Node.h"
#include "Light.h"
#include "Camera.h"
#include "vr/Shader.h"

#include "lab1/nodes/Group.h"
#include "lab1/visitors/RenderVisitor.h"

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
    Get all light sources
    \return A vector of lightsources
    */
    const LightVector& getLights();


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
    Add a new node
    \param node - A new node
    */
    void add(std::shared_ptr<Node>& node);

    void addObj(std::shared_ptr<Geometry>& obj);

    std::vector<std::shared_ptr<Geometry>> getObjects()
    {
      return m_objects;
    }
    
    /**
    Reset all nodes transform to its initial value
    */
    void resetTransform();

    /**
    Get a all nodes
    \return A vector containing all nodes
    */
    const NodeVector& getNodes();

    /**
    Get the i:th node
    \param i - Index in vector<
    \return A node
    */
    std::shared_ptr<Node> getNode(size_t i);

    /**
    Compute a bounding box for the whole scene
    \return A bounding box for the whole scene
    */
    BoundingBox calculateBoundingBox();

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

    RenderVisitor* getRenderVisitor()
    {
      return m_renderVisitor;
    }



  private:

    NodeVector m_nodes;
    LightVector m_lights;
    std::vector<std::shared_ptr<Geometry>> m_objects;

    GLint m_uniform_numberOfLights;
    Group *m_rootGroup;
    RenderVisitor *m_renderVisitor;
    std::shared_ptr<vr::Shader> m_shader;
    std::shared_ptr<Camera> m_camera;
  };
}