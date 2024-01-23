#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Node.h"
#include "Visitor.h"

class Geometry : public Node
{
    public:

        Geometry(const std::string& name = "") : Node(name) {}
        
        virtual void accept(Visitor& v) override
        {
            std::cerr << "Accept: " << getName() << std::endl;
            v.visit(*this);
        }

        virtual void render()
        {
            std::cerr << "Rendering" << std::endl;
        }
};

#endif