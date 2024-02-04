#include "lab1/visitors/RenderVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"

void RenderVisitor::visit(Group& group)
{
    if(m_stateStack.empty()) {
        m_stateStack.push(group.getState());
    } else {
        m_stateStack.push(m_stateStack.top()->merge(group.getState()));
    }

    for (auto child : group.getChildren()) {
        child->accept(*this);
    }
    
    m_stateStack.pop();
}

void RenderVisitor::visit(Transform& trans) 
{
    m_stateStack.push(m_stateStack.top()->merge(trans.getState()));
    
    m_transformStack.push(trans.getTransfromMat());
    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }
    m_transformStack.pop();
    m_stateStack.pop();
}

void RenderVisitor::visit(Geometry& geo)
{
    m_stateStack.push(m_stateStack.top()->merge(geo.getState()));
    std::cout << "Rendering w state: " << m_stateStack.top()->getStateName() << std::endl;
    m_stateStack.top()->apply(m_shader);
    geo.draw(m_shader, m_transformStack.top());
    m_stateStack.pop();
}