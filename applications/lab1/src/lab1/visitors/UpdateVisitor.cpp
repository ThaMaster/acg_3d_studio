#include "lab1/visitors/UpdateVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"

void UpdateVisitor::visit(Group& group) 
{
    if(group.getUpdateCallback()) 
        group.getUpdateCallback()->update(group);
    
    for (auto child : group.getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(Transform& trans) 
{
    if(trans.getUpdateCallback())
        trans.getUpdateCallback()->update(trans);
    

    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(Geometry& geo) 
{
    if(geo.getUpdateCallback())
        geo.getUpdateCallback()->update(geo);
}