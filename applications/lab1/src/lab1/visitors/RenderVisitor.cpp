#include "lab1/visitors/RenderVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"

void RenderVisitor::visit(Group& group)
{
    for (auto child : group.getChildren()) {
        child->accept(*this);
    }
}

void RenderVisitor::visit(Transform& trans) 
{
    m_transformStack.push(trans.getTransfromMat());
    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }
    m_transformStack.pop();
}

void RenderVisitor::visit(Geometry& geo)
{
    geo.draw(m_shader, m_transformStack.top());
}