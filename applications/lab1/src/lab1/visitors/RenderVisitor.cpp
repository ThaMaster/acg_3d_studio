#include "lab1/visitors/RenderVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"

void RenderVisitor::visit(Group& group)
{
    //std::cerr << "Visit: " << group.getName() << std::endl;
    for (auto child : group.getChildren()) {
        child->accept(*this);
    }
}

void RenderVisitor::visit(Transform& trans) 
{
    //std::cerr << "Visit: " << trans.getName() << std::endl;
    m_transformStack.push(trans.getTransfromMat());
    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }
    m_transformStack.pop();
}

void RenderVisitor::visit(Geometry& geo)
{
    //std::cerr << "Visit: " << geo.getName() << std::endl;
    geo.render(m_shader, m_transformStack.top());
    //std::cerr << "Has: " << geo.getMeshes().size() << " meshes!\n" << std::endl;
}