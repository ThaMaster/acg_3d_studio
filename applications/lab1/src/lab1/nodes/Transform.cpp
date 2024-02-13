#include "lab1/nodes/Transform.h"

void Transform::setTransformMat(glm::mat4 m) { m_transformMat = m; }
glm::mat4 Transform::getTransfromMat(void) { return m_transformMat; }

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