#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab2/RenderToTexture.h"
#include <vr/glErrorUtil.h>

RenderToTexture::RenderToTexture() 
{
    m_depthShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/depthRTT.vs", "shaders/depthRTT.fs"));
}

RenderToTexture::~RenderToTexture() {}

bool RenderToTexture::createRenderTarget(void)
{
    CHECK_GL_ERROR_LINE_FILE();
    glGenFramebuffers(1, &m_frameBuffer);

    glGenTextures(1, &m_depthTexture);
	glActiveTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    CHECK_GL_ERROR_LINE_FILE();
    return true;
}

void RenderToTexture::switchToFramebuffer()
{
    CHECK_GL_ERROR_LINE_FILE();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glViewport(0,0,2048,2048);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glCullFace(GL_FRONT);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::defaultBuffer() 
{
    CHECK_GL_ERROR_LINE_FILE();
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(0,0,1980,1080);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glCullFace(GL_BACK);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::apply(std::shared_ptr<vr::Shader> s)
{
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    s->setInt("shadowMap", 10);
    CHECK_GL_ERROR_LINE_FILE();
}

glm::mat4 RenderToTexture::calcLightMatrix(glm::vec4 l_pos, glm::vec2 nearFar)
{
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10, nearFar.x, nearFar.y);
    glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(l_pos), glm::vec3(0,0,0), glm::vec3(0,1,0));
    return depthProjectionMatrix * depthViewMatrix;
}

std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }