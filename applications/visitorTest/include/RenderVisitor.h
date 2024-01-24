#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include "Visitor.h"
#include "Group.h"
#include "Transform.h"
#include "Geometry.h"

class RenderVisitor : public Visitor
{
    public:
        
        void visit(Group& g) override
        {
            std::cerr << "Visit group: " << g.getName() << std::endl;
            for (auto child : g.getChildren()) {
                child->accept(*this);
            }
        }

        void visit(Transform& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            std::cerr << "--------------stack-push()" << std::endl;
            for (auto child : g.getChildren()) {
                child->accept(*this);
            }
            std::cerr << "--------------stack-pop()" << std::endl;

        }

        void visit(Geometry& g) override
        {
            std::cerr << "Visit geo: " << g.getName() << std::endl;
            g.render();
        }
};

#endif