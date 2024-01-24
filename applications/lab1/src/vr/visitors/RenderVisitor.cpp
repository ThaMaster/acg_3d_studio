#include "vr/visitors/RenderVisitor.h"
#include "vr/nodes/Group.h"
#include "vr/nodes/Transform.h"
#include "vr/nodes/Geometry.h"

void RenderVisitor::visit(Group& group)
{
    std::cerr << "Visit: " << group.getName() << std::endl;
    for (auto child : group.getChildren()) {
        child->accept(*this);
    }
}

void RenderVisitor::visit(Transform& trans) 
{
    std::cerr << "Visit: " << trans.getName() << std::endl;
    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }
}

void RenderVisitor::visit(Geometry& geo)
{
    std::cerr << "Visit: " << geo.getName() << std::endl;
    //geo.render();
}