#include "lab1/visitors/UpdateVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"

void UpdateVisitor::visit(Group& group) 
{
    if(group.getUpdateCallback())
    {

    }
}

void UpdateVisitor::visit(Transform& trans) 
{
    if(trans.getUpdateCallback())
    {

    }
}

void UpdateVisitor::visit(Geometry& geo) 
{
    if(geo.getUpdateCallback())
    {

    }
}