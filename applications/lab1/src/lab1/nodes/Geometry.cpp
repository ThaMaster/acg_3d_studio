#include "lab1/nodes/Geometry.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vr/Mesh.h>
#include <sstream>
#include <vr/glErrorUtil.h>

Geometry::~Geometry()
{
    if (m_vbo_vertices != 0)
        glDeleteBuffers(1, &m_vbo_vertices);

    if (m_vbo_normals != 0)
        glDeleteBuffers(1, &m_vbo_normals);

    if (m_ibo_elements != 0)
        glDeleteBuffers(1, &m_ibo_elements);
}

void Geometry::setMaterial(std::shared_ptr<vr::Material>& material) 
{ 
    m_material = material; 
}

std::shared_ptr<vr::Material> Geometry::getMaterial()
{
    return m_material;
}

bool Geometry::initShaders(std::shared_ptr<vr::Shader> shader)
{
    shader->use();
  
    const char* attribute_name;
    attribute_name = "vertex_position";
    m_attribute_v_coord = shader->getAttribute(attribute_name); // glGetAttribLocation(program, attribute_name);
    if (m_attribute_v_coord == -1)
        return false;

    attribute_name = "vertex_normal";
    m_attribute_v_normal = shader->getAttribute(attribute_name);
    if (m_attribute_v_normal == -1)
        return false;

    attribute_name = "vertex_texCoord";
    m_attribute_v_texCoords = shader->getAttribute(attribute_name);
    if (m_attribute_v_texCoords == -1)
        return false;

    return true;
}

vr::BoundingBox Geometry::calculateBoundingBox()
{
    vr::BoundingBox box;
    for (auto v : vertices)
    {
        glm::vec3 vTransformed = this->object2world * v;
        box.expand(vTransformed);
    }
    box = box * this->object2world;
    return box;
}

void Geometry::accept(NodeVisitor& v)
{
    //std::cerr << "Accept: " << getName() << std::endl;
    v.visit(*this);
}

void Geometry::add(std::shared_ptr<vr::Mesh>& mesh)
{
    m_meshes.push_back(mesh);
}

vr::MeshVector& Geometry::getMeshes()
{
    return m_meshes;
}

void Geometry::setInitialTransform(const glm::mat4& m)
{
    object2world = m_initialTransform = m;
}

void Geometry::resetTransform()
{
    object2world = m_initialTransform;
}

glm::mat4& Geometry::getObject2WorldMat()
{
    return object2world;
}

void Geometry::setObject2WorldMat(glm::mat4& m)
{
    object2world = m;
}

void Geometry::upload() 
{
    if (m_useVAO)
    {
        // Create a Vertex Array Object that will handle all VBO:s of this Mesh
        glGenVertexArrays(1, &m_vao);
        CHECK_GL_ERROR_LINE_FILE();
        glBindVertexArray(m_vao);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->vertices.size() > 0) {
        glGenBuffers(1, &this->m_vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]),
        this->vertices.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->normals.size() > 0) {
        glGenBuffers(1, &this->m_vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]),
            this->normals.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (this->texCoords.size() > 0) {
        glGenBuffers(1, &this->m_vbo_texCoords);
        glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
        glBufferData(GL_ARRAY_BUFFER, this->texCoords.size() * sizeof(this->texCoords[0]),
        this->texCoords.data(), GL_STATIC_DRAW);
        CHECK_GL_ERROR_LINE_FILE();
    }

    if (m_useVAO)
    {
        glEnableVertexAttribArray(m_attribute_v_coord);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
        glVertexAttribPointer(
        m_attribute_v_coord,  // attribute
        4,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
        );
        glDisableVertexAttribArray(m_attribute_v_coord);

        glEnableVertexAttribArray(m_attribute_v_normal);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
        glVertexAttribPointer(
        m_attribute_v_normal, // attribute
        3,                  // number of elements per vertex, here (x,y,z)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
        );
        glDisableVertexAttribArray(m_attribute_v_normal);

        glEnableVertexAttribArray(m_attribute_v_texCoords);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texCoords);
        glVertexAttribPointer(
        m_attribute_v_texCoords, // attribute
        2,                  // number of elements per vertex, here (x,y)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
        );
        glDisableVertexAttribArray(m_attribute_v_texCoords);

    }

    if (this->elements.size() > 0) {
        glGenBuffers(1, &this->m_ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(this->elements[0]),
        this->elements.data(), GL_STATIC_DRAW);
    }

    CHECK_GL_ERROR_LINE_FILE();

    if (m_useVAO)
    {
        // Now release VAO
        glEnableVertexAttribArray(0);  // Disable our Vertex Array Object  
        glBindVertexArray(0); // Disable our Vertex Buffer Object
        CHECK_GL_ERROR_LINE_FILE();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

/**
 * render()
 * ---------------
 * 
 * 
 */
void Geometry::draw(std::shared_ptr<vr::Shader> shader, glm::mat4 modelMat)
{
    //std::cerr << "Rendering" << std::endl;
    for(auto m : m_meshes)
    {
        m->initShaders(shader);
        m->upload();
        m->render(shader, (this->object2world * modelMat));
    }
}