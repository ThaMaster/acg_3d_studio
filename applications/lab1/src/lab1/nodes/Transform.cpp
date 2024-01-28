#include "lab1/nodes/Transform.h"

vr::BoundingBox Transform::calculateBoundingBox()
{
    vr::BoundingBox box;
    for(auto c : getChildren()) 
    {
        box.expand(c->calculateBoundingBox());
    }
    return box;
}

void Transform::accept(NodeVisitor& v)
{
    //std::cerr << "Accept: " << getName() << std::endl;
    v.visit(*this);
}