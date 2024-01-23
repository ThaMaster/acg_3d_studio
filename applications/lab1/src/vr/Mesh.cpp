// Important: Import glad as first gl header
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vr/Mesh.h>
#include <sstream>
#include <vr/glErrorUtil.h>

using namespace vr;


Mesh::Mesh(bool useVAO) : m_vbo_vertices(0), m_vbo_normals(0), m_vbo_texCoords(0), m_ibo_elements(0), object2world(glm::mat4(1)),
                           m_attribute_v_coord(-1), m_attribute_v_normal(-1), m_attribute_v_texCoords(-1), m_vao(-1), m_useVAO(useVAO)
{
  // Default material
  m_material = std::shared_ptr<Material>(new Material);
}

void Mesh::setMaterial(std::shared_ptr<Material>& material) 
{ 
  m_material = material; 
}


Mesh::~Mesh() 
{
  if (m_vbo_vertices != 0)
    glDeleteBuffers(1, &m_vbo_vertices);

  if (m_vbo_normals != 0)
    glDeleteBuffers(1, &m_vbo_normals);

  if (m_ibo_elements != 0)
    glDeleteBuffers(1, &m_ibo_elements);
}


bool Mesh::initShaders(std::shared_ptr<vr::Shader> shader)
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


/**
* Store object vertices, normals and/or elements in graphic card
* buffers
*/
void Mesh::upload() 
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
* Draw the object
*/
void Mesh::render(std::shared_ptr<vr::Shader> shader, const glm::mat4& modelMatrix)
{
  CHECK_GL_ERROR_LINE_FILE();
  if (m_useVAO) {
    glBindVertexArray(m_vao);
    CHECK_GL_ERROR_LINE_FILE();
  }

  if (normals.size() == 0)
  {
    draw_bbox(shader);
    return;
  }

  if (m_material)
    m_material->apply(shader);
  
  CHECK_GL_ERROR_LINE_FILE();

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

    if (this->m_vbo_normals != 0)
    {
      glEnableVertexAttribArray(m_attribute_v_normal);
      glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
      glVertexAttribPointer(
        m_attribute_v_normal, // attribute
        3,                  // number of elements per vertex, here (x,y,z)
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
        2,                  // number of elements per vertex, here (x,y,z)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
      );
    }
  }
  else {
    glEnableVertexAttribArray(m_attribute_v_coord);
    CHECK_GL_ERROR_LINE_FILE();
    glEnableVertexAttribArray(m_attribute_v_normal);
    CHECK_GL_ERROR_LINE_FILE();
    if (m_vbo_texCoords != 0)
      glEnableVertexAttribArray(m_attribute_v_texCoords);
    CHECK_GL_ERROR_LINE_FILE();
    
  }
  CHECK_GL_ERROR_LINE_FILE();

  /* Apply object's transformation matrix */
  glm::mat4 obj2World = modelMatrix * this->object2world;
  
  shader->setMat4("m", obj2World);

  /* 
  Transform normal vectors with transpose of inverse of upper left
  3x3 model matrix (ex-gl_NormalMatrix): 
  */
  glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(obj2World)));
  shader->setMat3("m_3x3_inv_transp", m_3x3_inv_transp);


  /* Push each element in buffer_vertices to the vertex shader */
  if (this->m_ibo_elements != 0) 
  {
    if (!m_useVAO)
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);

      GLuint size = GLuint(this->elements.size());
      glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
      CHECK_GL_ERROR_LINE_FILE();
  }
  else {
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->vertices.size());
  }

  if (this->m_vbo_normals != 0)
    glDisableVertexAttribArray(m_attribute_v_normal);

  if (this->m_vbo_vertices != 0)
    glDisableVertexAttribArray(m_attribute_v_coord);

  if (this->m_vbo_texCoords != 0)
    glDisableVertexAttribArray(m_attribute_v_texCoords);

  if (m_useVAO)
    glBindVertexArray(0);
}

BoundingBox Mesh::calculateBoundingBox()
{
  BoundingBox box;
  for (auto v : vertices)
  {
    glm::vec3 vTransformed = this->object2world * v;
    box.expand(vTransformed);
  }
  return box;
}

/**
* Draw object bounding box
*/
void Mesh::draw_bbox(std::shared_ptr<vr::Shader> shader)
{
  if (this->vertices.size() == 0)
    return;

  // Cube 1x1x1, centered on origin
  GLfloat vertices[] = {
    -0.5, -0.5, -0.5, 1.0,
    0.5, -0.5, -0.5, 1.0,
    0.5, 0.5, -0.5, 1.0,
    -0.5, 0.5, -0.5, 1.0,
    -0.5, -0.5, 0.5, 1.0,
    0.5, -0.5, 0.5, 1.0,
    0.5, 0.5, 0.5, 1.0,
    -0.5, 0.5, 0.5, 1.0,
  };

  GLuint vbo_vertices;
  glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  GLushort elements[] = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    0, 4, 1, 5, 2, 6, 3, 7
  };

  GLuint ibo_elements;
  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  GLfloat
    min_x, max_x,
    min_y, max_y,
    min_z, max_z;

  min_x = max_x = this->vertices[0].x;
  min_y = max_y = this->vertices[0].y;
  min_z = max_z = this->vertices[0].z;
  for (unsigned int i = 0; i < this->vertices.size(); i++) {
    if (this->vertices[i].x < min_x) min_x = this->vertices[i].x;
    if (this->vertices[i].x > max_x) max_x = this->vertices[i].x;
    if (this->vertices[i].y < min_y) min_y = this->vertices[i].y;
    if (this->vertices[i].y > max_y) max_y = this->vertices[i].y;
    if (this->vertices[i].z < min_z) min_z = this->vertices[i].z;
    if (this->vertices[i].z > max_z) max_z = this->vertices[i].z;
  }

  glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
  glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
  glm::mat4 transform = glm::scale(glm::mat4(1), size) * glm::translate(glm::mat4(1), center);

  /* Apply object's transformation matrix */
  glm::mat4 m = this->object2world * transform;
  shader->setMat4("m", m);

  CHECK_GL_ERROR_LINE_FILE();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glEnableVertexAttribArray(m_attribute_v_coord);
  glVertexAttribPointer(
    m_attribute_v_coord,  // attribute
    4,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
    );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
  glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(m_attribute_v_coord);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &vbo_vertices);
  glDeleteBuffers(1, &ibo_elements);
  CHECK_GL_ERROR_LINE_FILE();
}

std::shared_ptr<Material> Mesh::getMaterial()
{
  return m_material;
}
