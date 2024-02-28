#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include "lab2/RenderToTexture.h"
#include <vr/glErrorUtil.h>
#include <glm/gtx/string_cast.hpp>

RenderToTexture::RenderToTexture() 
{
    m_depthShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/depthRTT.vs", "shaders/depthRTT.fs", "shaders/depthRTT.gs"));
    m_depthTextures.resize(10);
    m_depthCubeMaps.resize(10);

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

    m_depthTextures[m_num_depth_components] = newDepthTexture;
    m_num_depth_components++;
    return true;
}

bool RenderToTexture::addDepthCubeMap()
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
    m_depthCubeMaps[m_num_depth_components] = newDepthCubemap;
    m_num_depth_components++;
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

void RenderToTexture::switchToDepthCubeMap(unsigned int unit)
{
    CHECK_GL_ERROR_LINE_FILE();
    m_depthShader->use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMaps[unit], 0);
    glViewport(0,0,2048,2048);
    glClear(GL_DEPTH_BUFFER_BIT);
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

void RenderToTexture::applyDepthMaps(std::shared_ptr<vr::Shader> s)
{
    int unit = 0;
    int i = 0;
    for(unit; unit < m_depthTextures.size(); unit++ ) {
        glActiveTexture(GL_TEXTURE10 + unit);
        glBindTexture(GL_TEXTURE_2D, m_depthTextures[i]);
        s->setInt(("shadowMaps[" + std::to_string(i) + "]").c_str(), 10 + unit);
        i++;
    }

    i = 0;
    for(unit; unit < m_depthCubeMaps.size(); unit++ ) {
        glActiveTexture(GL_TEXTURE10 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMaps[i]);
        s->setInt(("shadowCubeMaps[" + std::to_string(i) + "]").c_str(), 10 + unit);
        i++;
    }
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyDepthData(std::vector<glm::mat4> lms, glm::vec4 l_pos, float farPlane)
{
    if(l_pos.w == 0.0) {
        m_depthShader->setMat4("lightMatrices[0]", lms[0]);
    } else {
        for(int i = 0; i < 6; i++) {
            std::stringstream matStr;
            matStr << "lightMatrices[" << i << "]";
            std::string matPref = matStr.str();
            m_depthShader->setMat4(matPref, lms[i]);
        }
    }
    m_depthShader->setVec4("lightPos", l_pos);
    m_depthShader->setFloat("far_plane", farPlane);
}

std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }