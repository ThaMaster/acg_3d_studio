#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

#include "lab2/RenderToTexture.h"
#include <vr/glErrorUtil.h>

RenderToTexture::RenderToTexture() 
{
    m_depthShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/depthRTT.vs", "shaders/depthRTT.fs"));
    CHECK_GL_ERROR_LINE_FILE();
    glGenFramebuffers(1, &m_frameBuffer);
}

RenderToTexture::~RenderToTexture() {}

bool RenderToTexture::addRenderTarget()
{
    GLuint newDepthTexture;
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glGenTextures(1, &newDepthTexture);
	glActiveTexture(GL_TEXTURE10 + m_numRenderTargets);
    glBindTexture(GL_TEXTURE_2D, newDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, newDepthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    m_depthTextures.push_back(newDepthTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    CHECK_GL_ERROR_LINE_FILE();

    m_numRenderTargets++;
    return true;
}

void RenderToTexture::switchToDepthbuffer()
{
    CHECK_GL_ERROR_LINE_FILE();
    m_depthShader->use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glViewport(0,0,2048,2048);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::defaultBuffer() 
{
    CHECK_GL_ERROR_LINE_FILE();
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(0,0,1980,1080);
    glCullFace(GL_BACK);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::apply(std::shared_ptr<vr::Shader> s, unsigned int unit)
{
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE10 + unit);
    glBindTexture(GL_TEXTURE_2D, m_depthTextures[unit]);
    s->setInt("shadowMap", 10 + unit);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyLightMatrix(glm::mat4 lm)
{
    m_depthShader->setMat4("lightMatrix", lm);
}

std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }