#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Group.h"

/**
 * @brief A geometry node contains all the information regarding 
 *        a mesh which includes vertices, normals, texture coordinates
 *        and indices. The geometry node needs to first initialize
 *        the attribute locations in the used shader and then upload
 *        the geometry data to the buffers before being able to render 
 *        the scene.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 *      2024-03-01: v1.1, second version, now ignores to upload some values if the
 *                                        current render is a depth render.
 */
class Geometry : public Node
{        
    public:
        Geometry(const std::string& name = "") : Node(name) {}
        ~Geometry();
        
        virtual void accept(NodeVisitor& v) override;

        bool initShaders(std::shared_ptr<vr::Shader> shader);

        void setInitialTransform(const glm::mat4& m);
        void resetTransform();
        
        glm::mat4& getObject2WorldMat();
        void setObject2WorldMat(glm::mat4& m);

        void upload();
        void draw(std::shared_ptr<vr::Shader> shader, const glm::mat4& modelMatrix, bool depthPass);
        void draw_bbox(std::shared_ptr<vr::Shader> shader);
        virtual vr::BoundingBox calculateBoundingBox(glm::mat4 m) override;

        void setIsGround(bool b);

        std::vector<glm::vec4> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<GLushort> elements;
        std::vector<glm::vec3> tangents;
        
    private:

        GLuint m_vbo_vertices, m_vbo_normals, m_vbo_texCoords, m_ibo_elements;
        GLuint m_vbo_tangents = 0;
        GLuint m_vao;
        GLint m_attribute_v_coord, m_attribute_v_normal, m_attribute_v_texCoords;
        GLint m_attribute_v_tangents;
        bool m_is_ground = false;
        bool m_useVAO = true;
        glm::mat4 m_initialTransform;
        glm::mat4 object2world;
};

#endif