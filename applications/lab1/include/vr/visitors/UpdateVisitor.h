#ifndef UPDATEVISITOR_H
#define UPDATEVISITOR_H

#include "NodeVisitor.h"

class UpdateVisitor : public NodeVisitor
{
    public:
        void visit(Group& g) override;
        void visit(Transform& g) override;
        void visit(Geometry& g) override;   
    private:
};

#endif