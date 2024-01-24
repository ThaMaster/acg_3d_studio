#include "vr/nodes/Transform.h"

void Transform::accept(NodeVisitor& v)
{
    // v.getTransformStack().push_back(transformMat);
    Group::accept(v);
    // v.getTransformStack().pop_back();
}