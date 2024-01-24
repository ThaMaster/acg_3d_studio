#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Group.h"
#include "vr/Mesh.h"

class Geometry : public Node
{
    public:

        Geometry(const std::string& name = "") : Node(name) {}
        
        virtual void accept(NodeVisitor& v) override;

        virtual void render(std::shared_ptr<vr::Shader> shader);

        void add(std::shared_ptr<vr::Mesh>& mesh);
        vr::MeshVector& getMeshes();

        void setInitialTransform(const glm::mat4& m);
        void resetTransform();
        
        glm::mat4& getObject2WorldMat();
    private:

        vr::MeshVector m_meshes;
        glm::mat4 m_initialTransform;
        glm::mat4 object2world;
};

#endif