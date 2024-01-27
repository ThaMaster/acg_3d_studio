#pragma once

#include <memory>

#include "Material.h"
#include "Mesh.h"
#include "BoundingBox.h"
#include <glm/glm.hpp>
#include <vr/Shader.h>

namespace vr
{
  /**
  Simple class that store a number of meshes and draws it
  */
  class Node
  {

  public:
    /**
    /Name of the node
    */
    std::string name;

    /**
    Transformation for the nodes local coordinate system to the world
    */
    glm::mat4 object2world;

    /**
    Constructor
    */
    Node();

    /**
    Get all meshes

    \return A vector of meshes
    */
    MeshVector& getMeshes();

    /**
    Add a mesh to the vector of meshes
    \param mesh - A new mesh
    */
    void add(std::shared_ptr<Mesh>& mesh);

    /**
    Set an initial transformation that can be reset at a later point in time
    \param m - transformation matrix
    */
    void setInitialTransform(const glm::mat4& m);

    ~Node();

    void resetTransform();

    /**
    Draw the node (all of its meshes)
    \param program - The active program for which rendering will be performed
    */
    void render(std::shared_ptr<vr::Shader> shader);

    /// Calculate and return a bounding box for this Node based on its Mesh objects
    BoundingBox calculateBoundingBox();

  private:

    MeshVector m_meshes;
    glm::mat4 m_initialTransform;

  };
  typedef std::vector<std::shared_ptr<Node> > NodeVector;
}