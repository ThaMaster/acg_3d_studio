#include "lab1/nodes/Group.h"

std::vector<Node*> Group::getChildren() { return m_children; } 

vr::BoundingBox Group::calculateBoundingBox(glm::mat4 m)
{
    vr::BoundingBox box;
    for(auto c : getChildren())
    {
        box.expand(c->calculateBoundingBox(m));
    }
    return box;
}

void Group::addChild(Node* n)
{
    m_children.push_back(n);
}

void Group::accept(NodeVisitor& v)
{
    v.visit(*this);
}