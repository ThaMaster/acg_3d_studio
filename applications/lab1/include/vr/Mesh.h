#pragma once

#include <memory>

#include "Material.h"
#include "BoundingBox.h"
#include <glm/glm.hpp>
#include <string>
#include <vr/Shader.h>
namespace vr
{

  /// Simple class that store geometry data and draws it
  class Mesh
  {

  public:
    typedef std::vector<glm::vec2> vec2Vector;
    typedef std::vector<glm::vec3> vec3Vector;
    typedef std::vector<glm::vec4> vec4Vector;
    typedef std::vector<GLushort> GLushortVector;
    std::string name;


  public:
    vec4Vector vertices;
    vec3Vector normals;
    vec2Vector texCoords;
    GLushortVector elements;

    /// The transformation matrix that transforms from the local coordinate of the mesh to the world
    glm::mat4 object2world;

    /**
    Constructor
    \param useVAIf true Vertex Array Objects will be used for this mesh
    */
    Mesh(bool useVAO = true);

    void setMaterial(std::shared_ptr<Material>& material);
    std::shared_ptr<Material> getMaterial();

    ~Mesh();

    /**
    Initialize the shaders uniforms and attributes

    \param program - The program for which uniforms and attributes will be initialized
    \return true if it was successful.
    */
    bool initShaders(std::shared_ptr<vr::Shader> shader);


    /// Store object vertices, normals and/or elements in graphic card buffers
    void upload();


    /** Render the object
    \param program - Render the mesh using this program
    \param modelMatrix - The current model matrix
    */
    void render(std::shared_ptr<vr::Shader> shader, const glm::mat4& modelMatrix);

    /**
    Calculate and return a bounding box for this geometry based on its vertices

    \return The computed bounding box
    */
    BoundingBox calculateBoundingBox();

    /**
    Draw object bounding box
    */
    void draw_bbox(std::shared_ptr<vr::Shader> shader);

  private:

    std::shared_ptr<Material> m_material;

    GLuint m_vbo_vertices, m_vbo_normals, m_vbo_texCoords, m_ibo_elements;
    GLuint m_vao;
    GLint m_attribute_v_coord;
    GLint m_attribute_v_normal;
    GLint m_attribute_v_texCoords;

    //GLint m_uniform_m;
    //GLint m_uniform_m_3x3_inv_transp;

    bool m_useVAO;
  };
  typedef std::vector<std::shared_ptr<Mesh> > MeshVector;

}