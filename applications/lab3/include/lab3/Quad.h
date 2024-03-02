#ifndef QUAD_H
#define QUAD_H

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <vr/Shader.h>


class Quad 
{
    public:
        Quad() {};
        ~Quad();

        void drawQuad(std::shared_ptr<vr::Shader> shader);
        void uploadQuad();
        bool initShaders(std::shared_ptr<vr::Shader> shader);

        void setVertices(std::vector<glm::vec4> vs);
        void setTexCoords(std::vector<glm::vec2> ts);
        void setElements(std::vector<GLushort> es);
        
    private:
        GLuint m_vbo_vertices, m_vbo_texCoords, m_ibo_elements;
        GLuint m_vao;

        GLint m_attribute_v_coord, m_attribute_v_texCoords;

        bool m_useVAO = true;

        std::vector<glm::vec4> m_vertices;
        std::vector<glm::vec2> m_texCoords;
        std::vector<GLushort> m_elements;
};

#endif