#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include "visitor.h"
#include "group.h"
#include "transform.h"
#include "geometry.h"

class RenderVisitor : public Visitor
{
    public:
        
        void visit(Group& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            for (auto child : g.getChildren()) {
                child->accept(*this);
            }
        }

        void visit(Transform& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            for (auto child : g.getChildren()) {
                child->accept(*this);
            }
        }

        void visit(Geometry& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            g.render();
        }
};

#endif