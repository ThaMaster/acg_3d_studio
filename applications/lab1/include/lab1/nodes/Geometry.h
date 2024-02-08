#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Group.h"

class Geometry : public Node
{        
    public:

        std::vector<glm::vec4> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<GLushort> elements;

        Geometry(const std::string& name = "") : Node(name) {}
        ~Geometry();
        
        virtual void accept(NodeVisitor& v) override;

        bool initShaders(std::shared_ptr<vr::Shader> shader);

        void setInitialTransform(const glm::mat4& m);
        void resetTransform();
        
        glm::mat4& getObject2WorldMat();
        void setObject2WorldMat(glm::mat4& m);

        void upload();
        void draw(std::shared_ptr<vr::Shader> shader, const glm::mat4& modelMatrix);
        void draw_bbox(std::shared_ptr<vr::Shader> shader);
        virtual vr::BoundingBox calculateBoundingBox() override;

    private:
        GLuint m_vbo_vertices, m_vbo_normals, m_vbo_texCoords, m_ibo_elements;
        GLuint m_vao;
        GLint m_attribute_v_coord;
        GLint m_attribute_v_normal;
        GLint m_attribute_v_texCoords;

        //GLint m_uniform_m;
        //GLint m_uniform_m_3x3_inv_transp;

        bool m_useVAO = true;
        glm::mat4 m_initialTransform;
        glm::mat4 object2world;
};

#endif