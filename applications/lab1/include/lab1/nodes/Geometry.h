#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Group.h"
#include "vr/Mesh.h"

class Geometry : public M_Node
{
    public:

        Geometry(const std::string& name = "") : M_Node(name) {}
        
        virtual void accept(NodeVisitor& v) override;

        virtual void render(std::shared_ptr<vr::Shader> shader, glm::mat4 transformMat);

        void add(std::shared_ptr<vr::Mesh>& mesh);
        vr::MeshVector& getMeshes();

        void setInitialTransform(const glm::mat4& m);
        void resetTransform();
        
        glm::mat4& getObject2WorldMat();
        void setObject2WorldMat(glm::mat4& m);

    private:

        vr::MeshVector m_meshes;
        glm::mat4 m_initialTransform;
        glm::mat4 object2world;
};

#endif