#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include "lab3/RenderToTexture.h"
#include <vr/glErrorUtil.h>
#include <glm/gtx/string_cast.hpp>

RenderToTexture::RenderToTexture() 
{
    m_depthShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/depthRTT.vs", "shaders/depthRTT.fs", "shaders/depthRTT.gs"));
    m_gShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/gBuffer.vs", "shaders/gBuffer.fs"));
    for(int i = 0; i < 10; i++) {
        m_depthTextures.push_back(-1);
        m_depthCubeMaps.push_back(-1);
    }
    CHECK_GL_ERROR_LINE_FILE();
    glGenFramebuffers(1, &m_depthBuffer);
    glGenFramebuffers(1, &m_gBuffer);
    initGBufferAttribs();
}

RenderToTexture::~RenderToTexture() {}

void RenderToTexture::addDepthMap()
{
    GLuint newDepthTexture;
    glGenTextures(1, &newDepthTexture);
	glActiveTexture(GL_TEXTURE10 + m_num_depth_components);
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
}

void RenderToTexture::addDepthCubeMap()
{
    GLuint newDepthCubemap;
    glGenTextures(1, &newDepthCubemap);
	glActiveTexture(GL_TEXTURE10 + m_num_depth_components);
    glBindTexture(GL_TEXTURE_CUBE_MAP, newDepthCubemap);
    for(unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    m_depthCubeMaps[m_num_depth_components] = newDepthCubemap;
    m_num_depth_components++;
}

void RenderToTexture::switchToDepthTexture(unsigned int unit)
{
    CHECK_GL_ERROR_LINE_FILE();
    m_depthShader->use();
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextures[unit], 0);
    glViewport(0,0,2048,2048);
    glClear(GL_DEPTH_BUFFER_BIT);
    CHECK_GL_ERROR_LINE_FILE();
}

/**
 * @brief Function for switching to the depth frame buffer and
 *        bind the depth cube map texture that should be created
 *        for a specific light.
 * 
 * @param unit The texture unit that represents the
 *             current lights depth texture / depth cubemap.
 */
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

/**
 * @brief Function for switching to the default frame buffer
 *        that is used for rendering the scene as usual. This
 *        function should be called AFTER the different depth
 *        maps have been rendered and applied to textures.
 */
void RenderToTexture::defaultBuffer() 
{
    CHECK_GL_ERROR_LINE_FILE();
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(0,0,1980,1080);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    CHECK_GL_ERROR_LINE_FILE();
}

/**
 * @brief Function for applying all the depth textures and depth
 *        cube maps to a specific shader. Both textures will start
 *        from texture unit 10 but will not be applied to the index
 *        of the array if it is not set by the system.
 * 
 * @param shader The shader to set the depth maps.
 */
void RenderToTexture::applyDepthMaps(std::shared_ptr<vr::Shader> shader)
{
    int i = 0;
    for(int unit = 0; unit < m_num_depth_components; unit++ ) {

        if(m_depthTextures[unit] != -1) {
            applyDepthTexture(shader, unit, i);
        }

        if(m_depthCubeMaps[unit] != -1) {
            applyDepthCubeMap(shader, unit, i);
        }
        i++;
    }
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyDepthTexture(std::shared_ptr<vr::Shader> shader, int texUnit, int texPos)
{
    glActiveTexture(GL_TEXTURE10 + texUnit);
    glBindTexture(GL_TEXTURE_2D, m_depthTextures[texUnit]);
    shader->setInt(("shadowMaps[" + std::to_string(texPos) + "]").c_str(), 10 + texUnit);    
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyDepthCubeMap(std::shared_ptr<vr::Shader> shader, int texUnit, int texPos)
{
    glActiveTexture(GL_TEXTURE10 + texUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMaps[texUnit]);
    shader->setInt(("shadowCubeMaps[" + std::to_string(texPos) + "]").c_str(), 10 + texUnit);
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
        m_depthShader->setFloat("far_plane", farPlane);
    }
    m_depthShader->setVec4("lightPos", l_pos);
}

void RenderToTexture::initGBufferAttribs(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    // - position color buffer
    glGenTextures(1, &m_gPosition);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);
    
    // - normal color buffer
    glGenTextures(1, &m_gNormal);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);
    
    // - color + specular color buffer
    glGenTextures(1, &m_gAlbedoSpec);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1920, 1080);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Framebuffer not complete!" << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderToTexture::bindGBuffer()
{
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderToTexture::applyPositionTexture(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    shader->setInt("quadTexture", 10);
    shader->setInt("numTexVals", 4);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyNormalTexture(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    shader->setInt("quadTexture", 11);
    shader->setInt("numTexVals", 3);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyDiffuseTexture(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    shader->setInt("quadTexture", 12);
    shader->setInt("numTexVals", 3);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applySpecularTexture(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    shader->setInt("quadTexture", 12);
    shader->setInt("numTexVals", 1);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyGAttribs(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
    CHECK_GL_ERROR_LINE_FILE();
    
}

std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }
std::shared_ptr<vr::Shader> RenderToTexture::getGBufferShader(void) { return m_gShader; }