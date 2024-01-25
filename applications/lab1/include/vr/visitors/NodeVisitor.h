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

class NodeVisitor
{
    public:
        virtual void visit(Group&) = 0;
        virtual void visit(Transform&) = 0;
        virtual void visit(Geometry&) = 0;

    private:
        std::stack<glm::mat4> m_transformStack;
};

#endif