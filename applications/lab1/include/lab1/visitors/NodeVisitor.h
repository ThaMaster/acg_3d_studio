#ifndef NODEVISITOR_H
#define NODEVISITOR_H

#include <memory>
#include <iostream>
#include <vector>
#include <stack>
#include <glm/glm.hpp>

class Group;
class Transform;
class Geometry;
class LOD;

class NodeVisitor
{
    public:
        virtual void visit(Group&) = 0;
        virtual void visit(Transform&) = 0;
        virtual void visit(Geometry&) = 0;
        virtual void visit(LOD&) = 0;

    private:
        
};

#endif