#include "lab1/nodes/Transform.h"

void Transform::accept(NodeVisitor& v)
{
    std::cerr << "Accept: " << getName() << std::endl;
    v.visit(*this);
}