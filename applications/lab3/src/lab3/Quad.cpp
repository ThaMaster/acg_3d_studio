#include "lab3/Quad.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vr/glErrorUtil.h>
#include <iostream>
#include <sstream>

Quad::Quad()
{
  m_quadShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/quadShader.vs", "shaders/quadShader.fs"));
}

Quad::~Quad()
{
    if (m_vbo_vertices != 0)
        glDeleteBuffers(1, &m_vbo_vertices);

    if (m_vbo_texCoords != 0)
        glDeleteBuffers(1, &m_vbo_texCoords);
}

std::shared_ptr<vr::Shader> Quad::getQuadShader() { return m_quadShader; }
void Quad::setVertices(std::vector<glm::vec4> vs) { m_vertices = vs; }
void Quad::setTexCoords(std::vector<glm::vec2> ts) { m_texCoords = ts; }
void Quad::setElements(std::vector<GLushort> es) { m_elements = es; }

bool Quad::initShaders()
{
  m_quadShader->use();

  const char* attribute_name;
  attribute_name = "vertex_position";
  m_attribute_v_coord = m_quadShader->getAttribute(attribute_name);
  if (m_attribute_v_coord == -1)
      return false;

  attribute_name = "vertex_texCoord";
  m_attribute_v_texCoords = m_quadShader->getAttribute(attribute_name);
  if (m_attribute_v_texCoords == -1)
      return false;
  
  return true;
}

void Quad::uploadQuad() 
{
  if (m_useVAO)
  {
    // Create a Vertex Array Object that will handle all VBO:s of this Mesh
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR_LINE_FILE();
  }

  if (this->m_vertices.size() > 0) {
    glGenBuffers(1, &this->m_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(this->m_vertices[0]),
    this->m_vertices.data(), GL_STATIC_DRAW);
    CHECK_GL_ERROR_LINE_FILE();
  }

  if (this->m_texCoords.size() > 0) {
    glGenBuffers(1, &this->m_vbo_texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
    glBufferData(GL_ARRAY_BUFFER, this->m_texCoords.size() * sizeof(this->m_texCoords[0]),
    this->m_texCoords.data(), GL_STATIC_DRAW);
    CHECK_GL_ERROR_LINE_FILE();
  }
  if (m_useVAO)
  {
    CHECK_GL_ERROR_LINE_FILE();

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
    CHECK_GL_ERROR_LINE_FILE();
      
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
    CHECK_GL_ERROR_LINE_FILE();
    
  }
  if (this->m_elements.size() > 0) {
      glGenBuffers(1, &this->m_ibo_elements);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_elements.size() * sizeof(this->m_elements[0]),
      this->m_elements.data(), GL_STATIC_DRAW);
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

void Quad::drawQuad()
{
  CHECK_GL_ERROR_LINE_FILE();
  if (m_useVAO) {
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR_LINE_FILE();
  }

  if (!m_useVAO)
  {
    if (this->m_vbo_vertices != 0) 
    {
      glEnableVertexAttribArray(m_attribute_v_coord);
      glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
      glVertexAttribPointer(
        m_attribute_v_coord,// attribute
        4,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
        );
    }
    if (this->m_vbo_texCoords != 0)
    {
      glEnableVertexAttribArray(m_attribute_v_texCoords);
      glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
      glVertexAttribPointer(
        m_attribute_v_texCoords, // attribute
        2,                  // number of elements per vertex, here (x,y)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
      );
    }

  } else {
    glEnableVertexAttribArray(m_attribute_v_coord);
    CHECK_GL_ERROR_LINE_FILE();
    if (m_vbo_texCoords != 0)
      glEnableVertexAttribArray(m_attribute_v_texCoords);
    CHECK_GL_ERROR_LINE_FILE();
  }
  
  CHECK_GL_ERROR_LINE_FILE();

  /* Push each element in buffer_vertices to the vertex shader */
  if (this->m_ibo_elements != 0) 
  {
    if (!m_useVAO)
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
    CHECK_GL_ERROR_LINE_FILE();
    GLuint size = GLuint(this->m_elements.size());
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
    CHECK_GL_ERROR_LINE_FILE();
  }
  else {
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->m_vertices.size());
  }

  if (this->m_vbo_vertices != 0)
    glDisableVertexAttribArray(m_attribute_v_coord);

  if (this->m_vbo_texCoords != 0)
    glDisableVertexAttribArray(m_attribute_v_texCoords);

  if (m_useVAO)
    glBindVertexArray(0);
}