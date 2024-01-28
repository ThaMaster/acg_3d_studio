#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Group.h"
#include "vr/Mesh.h"

class Geometry : public M_Node
{
    public:

        std::vector<glm::vec4> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;
        std::vector<GLushort> elements;

        Geometry(const std::string& name = "") : M_Node(name) {}
        ~Geometry();
        
        virtual void accept(NodeVisitor& v) override;
        virtual vr::BoundingBox calculateBoundingBox() override;

        bool initShaders(std::shared_ptr<vr::Shader> shader);

        virtual void draw(std::shared_ptr<vr::Shader> shader, glm::mat4 modelMat);

        void add(std::shared_ptr<vr::Mesh>& mesh);
        vr::MeshVector& getMeshes();

        void setInitialTransform(const glm::mat4& m);
        void resetTransform();
        
        glm::mat4& getObject2WorldMat();
        void setObject2WorldMat(glm::mat4& m);

        void setMaterial(std::shared_ptr<vr::Material>& material);
        std::shared_ptr<vr::Material> getMaterial();

        void upload();

    private:
        std::shared_ptr<vr::Material> m_material;

        GLuint m_vbo_vertices, m_vbo_normals, m_vbo_texCoords, m_ibo_elements;
        GLuint m_vao;
        GLint m_attribute_v_coord;
        GLint m_attribute_v_normal;
        GLint m_attribute_v_texCoords;

        //GLint m_uniform_m;
        //GLint m_uniform_m_3x3_inv_transp;

        bool m_useVAO;
        vr::MeshVector m_meshes;
        glm::mat4 m_initialTransform;
        glm::mat4 object2world;
};

#endif