#include "lab1/visitors/RenderVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"
#include "lab1/nodes/LOD.h"

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

    if(m_transformStack.empty()) {
        m_transformStack.push(trans.getTransfromMat());
    } else {
        m_transformStack.push(m_transformStack.top() * trans.getTransfromMat());
    }

    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }

    m_transformStack.pop();
    m_stateStack.pop();
}

void RenderVisitor::visit(Geometry& geo)
{
    m_stateStack.push(m_stateStack.top()->merge(geo.getState()));
    auto state = m_stateStack.top();
    state->apply();
    m_camera->apply(state->getShader());
    geo.draw(state->getShader(), m_transformStack.top());
    m_stateStack.pop();
}

void RenderVisitor::visit(LOD& lod)
{
    m_stateStack.push(m_stateStack.top()->merge(lod.getState()));

    float distToCamera = lod.getDistanceToCamera(glm::vec4(m_camera->getPosition(), 1.0f));
    auto selectedObj = lod.getObjectToRender(distToCamera);

    if(selectedObj) {
        selectedObj->accept(*this);
    }

    m_stateStack.pop();
}