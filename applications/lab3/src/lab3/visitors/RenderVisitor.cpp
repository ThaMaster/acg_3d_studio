#include "lab3/visitors/RenderVisitor.h"
#include "lab3/nodes/Group.h"
#include "lab3/nodes/Transform.h"
#include "lab3/nodes/Geometry.h"
#include "lab3/nodes/LOD.h"
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
    if(m_depthPass) {
        m_rtt->applyDepthData(m_currLight->getLightMatrices(), m_currLight->getPosition(), m_camera->getNearFar().y);
        geo.draw(m_rtt->getDepthShader(), m_transformStack.top(), m_depthPass);
    } else if(m_gBufferPass) {
        state->setShader(m_rtt->getGBufferShader());
        state->apply(false);
        m_camera->apply(m_rtt->getGBufferShader());
        geo.draw(m_rtt->getGBufferShader(), m_transformStack.top(), false);
    } else {
        m_mainQuad->getQuadShader()->use();
        state->applyLights(m_mainQuad->getQuadShader());
        m_camera->applyInv(m_mainQuad->getQuadShader());
        m_rtt->applyGAttribs(m_mainQuad->getQuadShader());
        // state->getShader()->setBool("useShadowMap", m_useShadowMap);
        // state->getShader()->setFloat("far_plane", getCamera()->getNearFar().y);
        // m_camera->apply(state->getShader());

        /* if(m_useShadowMap)
            m_rtt->applyDepthMaps(state->getShader()); */
        m_mainQuad->drawQuad();
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

void RenderVisitor::setGBufferPass(bool b) { m_gBufferPass = b; }
bool RenderVisitor::getGBufferPass(void) { return m_gBufferPass; }

void RenderVisitor::setUseShadowMap(bool b) { m_useShadowMap = b; }
bool RenderVisitor::getUseShadowMap(void) { return m_useShadowMap; }

void RenderVisitor::setRTT(std::shared_ptr<RenderToTexture> rtt) {m_rtt = rtt; }
std::shared_ptr<RenderToTexture> RenderVisitor::getRTT(void) { return m_rtt; }

void RenderVisitor::setMainQuad(std::shared_ptr<Quad> quad) { m_mainQuad = quad; }
void RenderVisitor::setCurrentLight(std::shared_ptr<vr::Light> light) { m_currLight = light; }

void RenderVisitor::setLightShader(std::shared_ptr<vr::Shader> s) { m_lightShader = s; }