#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab2/RenderToTexture.h"

RenderToTexture::RenderToTexture() 
{
    m_depthShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/depthRTT.vs", "shaders/depthRTT.fs"));
}

RenderToTexture::~RenderToTexture() {}

bool RenderToTexture::createRenderTarget(void)
{
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    glGenTextures(1, &m_depthTexture);
	glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    return true;
}

void RenderToTexture::switchToFramebuffer()
{
    glViewport(0,0,1024,1024);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderToTexture::defaultBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(0,0,1980,1080);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderToTexture::apply(std::shared_ptr<vr::Shader> s)
{
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    s->setInt("shadowMap", 11);
}

glm::mat4 RenderToTexture::calcLightMatrix(glm::vec4 l_pos, glm::vec2 nearFar)
{
    // Spotlight or directional light?
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10, nearFar.x, nearFar.y);
	glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(l_pos), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    return depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
}

std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }