#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include "Node.h"
#include "NodeVisitor.h"
#include "Mesh.h"

class Geometry : public Node
{
    public:

        Geometry(const std::string& name = "") : Node(name) {}
        
        virtual void accept(NodeVisitor& v) override
        {
            std::cerr << "Accept: " << getName() << std::endl;
            v.visit(*this);
        }

        virtual void render()
        {
            for (auto m : m_meshes)
            {
               // m->render();
            }
            std::cerr << "Rendering" << std::endl;
        }
        
    private:

        std::string name;
        vr::MeshVector m_meshes;
};

#endif