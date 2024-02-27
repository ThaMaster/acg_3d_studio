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
    glGenFramebuffers(1, &m_depthBuffer);
}

RenderToTexture::~RenderToTexture() {}

bool RenderToTexture::addDepthMap()
{
    GLuint newDepthTexture;
    glGenTextures(1, &newDepthTexture);
    glBindTexture(GL_TEXTURE_2D, newDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    m_depthTextures.push_back(newDepthTexture);
    return true;
}

bool RenderToTexture::addDepthCubemap()
{
    GLuint newDepthCubemap;
    glGenTextures(1, &newDepthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, newDepthCubemap);
    for(unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
    m_depthCubeMaps.push_back(newDepthCubemap);
    return true;
}

void RenderToTexture::switchToDepthTexture(unsigned int unit)
{
    CHECK_GL_ERROR_LINE_FILE();
    m_depthShader->use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextures[unit], 0);
    glViewport(0,0,2048,2048);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::switchToDepthCubemap(unsigned int unit)
{
    CHECK_GL_ERROR_LINE_FILE();
    m_depthShader->use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMaps[unit], 0);
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
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::apply(std::shared_ptr<vr::Shader> s)
{
    int unit = 0;
    for(auto tex : m_depthTextures) {
        glActiveTexture(GL_TEXTURE10 + unit);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(s->program(), ("shadowMaps[" + std::to_string(unit) + "]").c_str()), 10 + unit);
        unit++;
    }
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyLightMatrix(glm::mat4 lm)
{
    m_depthShader->setMat4("lightMatrix", lm);
}

std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }