#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>

#include "NodeVisitor.h"

class RenderVisitor : public NodeVisitor
{
    public:
        void visit(Group& g) override;
        void visit(Transform& g) override;
        void visit(Geometry& g) override;        
};

#endif