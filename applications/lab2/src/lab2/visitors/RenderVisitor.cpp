#include "lab2/visitors/RenderVisitor.h"
#include "lab2/nodes/Group.h"
#include "lab2/nodes/Transform.h"
#include "lab2/nodes/Geometry.h"
#include "lab2/nodes/LOD.h"
#include "vr/Light.h"
#include <vr/glErrorUtil.h>
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
    glm::mat4 lightMatrix = m_rtt->calcLightMatrix(state->getLights()[0]->getPosition(), m_camera->getNearFar());
    if(m_depthPass) {
        m_rtt->getDepthShader()->setMat4("lightMatrix", lightMatrix);
        geo.draw(m_rtt->getDepthShader(), m_transformStack.top(), m_depthPass);
    } else {
        state->apply();
        m_rtt->apply(state->getShader());
        state->getShader()->setBool("useShadowMap", m_useShadowMap);
        state->getShader()->setMat4("lightMatrix", lightMatrix);
        m_camera->apply(state->getShader());
        geo.draw(state->getShader(), m_transformStack.top(), m_depthPass);
    }

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

void RenderVisitor::setCamera(std::shared_ptr<vr::Camera> c) { m_camera = c; }
std::shared_ptr<vr::Camera> RenderVisitor::getCamera(void) { return m_camera; }

void RenderVisitor::setDepthPass(bool b) { m_depthPass = b; }
bool RenderVisitor::getDepthPass(void) { return m_depthPass; }
void RenderVisitor::setUseShadowMap(bool b) { m_useShadowMap = b; }
bool RenderVisitor::getUseShadowMap(void) { return m_useShadowMap; }

void RenderVisitor::setRTT(std::shared_ptr<RenderToTexture> rtt) {m_rtt = rtt; }
std::shared_ptr<RenderToTexture> RenderVisitor::getRTT(void) { return m_rtt; }