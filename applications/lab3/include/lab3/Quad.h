#ifndef QUAD_H
#define QUAD_H

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <vr/Shader.h>

/**
 * @brief A class for rendering quads to the screen. Since
 *        the project now uses deferred rendering this class
 *        is essential for showcasing the different stored
 *        textures. The class is a more basic version of the
 *        geometry class since a quad only needs vertex positions,
 *        indices and texture coordinates.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-03-19: v1.0, first version. 
 */
class Quad 
{
    public:
        Quad();
        ~Quad();

        void drawQuad(void);

        void uploadQuad(void);
        bool initShaders();

        void setVertices(std::vector<glm::vec4> vs);
        void setTexCoords(std::vector<glm::vec2> ts);
        void setElements(std::vector<GLushort> es);

        std::shared_ptr<vr::Shader> getQuadShader(void);
        void setQuadShader(std::shared_ptr<vr::Shader> s) { m_quadShader = s; }
    
    private:
        std::shared_ptr<vr::Shader> m_quadShader;

        GLuint m_vbo_vertices, m_vbo_texCoords, m_ibo_elements;
        GLuint m_vao;

        GLint m_attribute_v_coord, m_attribute_v_texCoords;

        bool m_useVAO = true;

        std::vector<glm::vec4> m_vertices;
        std::vector<glm::vec2> m_texCoords;
        std::vector<GLushort> m_elements;

        glm::mat4 quadTransform = glm::mat4(1.0f);
};

#endif