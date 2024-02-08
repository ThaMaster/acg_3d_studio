#include "lab1/nodes/Transform.h"

vr::BoundingBox Transform::calculateBoundingBox(glm::mat4 m)
{
    vr::BoundingBox box;
    for(auto c : getChildren()) 
    {
        box.expand(c->calculateBoundingBox(m * m_transformMat));
    }
    return box;
}

void Transform::accept(NodeVisitor& v)
{
    v.visit(*this);
}