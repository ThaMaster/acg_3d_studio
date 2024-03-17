#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include "lab3/RenderToTexture.h"
#include <vr/glErrorUtil.h>
#include <glm/gtx/string_cast.hpp>

RenderToTexture::RenderToTexture() 
{
    m_gShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/gBuffer.vs", "shaders/gBuffer.fs"));
    m_postFXShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/PFX/postFX.vs", "shaders/PFX/postFX.fs"));
    m_blurShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/PFX/blur.vs", "shaders/PFX/blur.fs"));
    m_depthShader = std::shared_ptr<vr::Shader>(new vr::Shader("shaders/depthRTT.vs", "shaders/depthRTT.fs", "shaders/depthRTT.gs"));

    for(int i = 0; i < 32; i++) {
        m_depthTextures.push_back(-1);
        m_depthCubeMaps.push_back(-1);
    }
    
    m_gTextures.resize(4);
    m_colorBuffers.resize(2);

    m_bloomBlurFBOs.resize(2);
    m_bloomBlurCBs.resize(2);

    m_dofBlurFBOs.resize(2);
    m_dofBlurCBs.resize(2);

    CHECK_GL_ERROR_LINE_FILE();
    glGenFramebuffers(1, &m_depthBuffer);
    glGenFramebuffers(1, &m_gBuffer);
    initFrameBuffer();
    initBlurBuffers();
    initGBufferAttribs();
}

RenderToTexture::~RenderToTexture() {}

void RenderToTexture::addDepthMap()
{
    GLuint newDepthTexture;
    glGenTextures(1, &newDepthTexture);
	glActiveTexture(GL_TEXTURE5 + m_num_depth_components);
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
	glActiveTexture(GL_TEXTURE5 + m_num_depth_components);
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
    glViewport(0,0,1920,1080);
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
    glActiveTexture(GL_TEXTURE5 + texUnit);
    glBindTexture(GL_TEXTURE_2D, m_depthTextures[texUnit]);
    shader->setInt(("shadowMaps[" + std::to_string(texPos) + "]").c_str(), 5 + texUnit);    
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyDepthCubeMap(std::shared_ptr<vr::Shader> shader, int texUnit, int texPos)
{
    glActiveTexture(GL_TEXTURE5 + texUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMaps[texUnit]);
    shader->setInt(("shadowCubeMaps[" + std::to_string(texPos) + "]").c_str(), 5 + texUnit);
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

void RenderToTexture::initFrameBuffer(void)
{
    glGenFramebuffers(1, &m_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

    for(int i = 0; i < 3; i++) {
        glGenTextures(1, &m_colorBuffers[i]);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorBuffers[i], 0);
    }

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    // finally check if framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: (frameBuffer) Framebuffer not complete!" << std::endl;
        exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderToTexture::initBlurBuffers()
{
    for(int i = 0; i < 2; i++) {
        glGenFramebuffers(1, &m_bloomBlurFBOs[i]);
        glGenTextures(1, &m_bloomBlurCBs[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, m_bloomBlurFBOs[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_bloomBlurCBs[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bloomBlurCBs[i], 0);
    }
    for(int i = 0; i < 2; i++) {
        glGenFramebuffers(1, &m_dofBlurFBOs[i]);
        glGenTextures(1, &m_dofBlurCBs[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, m_dofBlurFBOs[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_dofBlurCBs[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_dofBlurCBs[i], 0);
    }
}

void RenderToTexture::initGBufferAttribs(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    for(int i = 0; i < 4; i++) {
        glGenTextures(1, &m_gTextures[i]);
        // - position color buffer
        glActiveTexture(GL_TEXTURE10 + i);
        glBindTexture(GL_TEXTURE_2D, m_gTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_gTextures[i], 0);
    }

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    glGenTextures(1, &m_depthTexture);
	glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: (gBuffer) Framebuffer not complete!" << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderToTexture::bindFB()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glBindTexture(GL_TEXTURE_2D, m_gTextures[0]);
    shader->setInt("quadTexture", 10);
    shader->setInt("numTexVals", 4);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyNormalTexture(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[1]);
    shader->setInt("quadTexture", 11);
    shader->setInt("numTexVals", 3);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyDiffuseTexture(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[2]);
    shader->setInt("quadTexture", 12);
    shader->setInt("numTexVals", 3);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applySpecularTexture(std::shared_ptr<vr::Shader> shader)
{
    glm::vec4 valuePosition = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[2]);
    shader->setInt("quadTexture", 12);
    shader->setInt("numTexVals", 1);
    shader->setVec4("valPos", valuePosition);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyCamDepth(std::shared_ptr<vr::Shader> shader)
{
    glm::vec4 valuePosition = glm::vec4(1.0, 0.0, 0.0, 0.0);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    shader->setInt("quadTexture", 14);
    shader->setInt("numTexVals", 1);
    shader->setVec4("valPos", valuePosition);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyLightDepth(std::shared_ptr<vr::Shader> shader, int lightIdx, glm::vec4 l_pos, float farPlane)
{
    if(m_depthTextures[lightIdx] != -1) {
        glm::vec4 valuePosition = glm::vec4(1.0, 0.0, 0.0, 0.0);
        glBindTexture(GL_TEXTURE_2D, m_depthTextures[lightIdx]);
        glActiveTexture(GL_TEXTURE5 + lightIdx);
        shader->setInt("quadTexture", 5 + lightIdx);
        shader->setInt("numTexVals", 1);
        shader->setVec4("valPos", valuePosition);
        shader->setBool("useDepthCubeMap", false);
    }
    CHECK_GL_ERROR_LINE_FILE();

    if(m_depthCubeMaps[lightIdx] != -1) {
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, m_gTextures[0]);
        shader->setInt("gPosition", 10);

        glActiveTexture(GL_TEXTURE5 + lightIdx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMaps[lightIdx]);
        shader->setInt("quadCubeMap", 5 + lightIdx);
        shader->setVec4("lPosition", l_pos);
        shader->setBool("useDepthCubeMap", true);
    }
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::usePostFXShader(bool bloom, bool dof, bool horizontal)
{
    m_postFXShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffers[0]);
    m_postFXShader->setInt("scene", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_bloomBlurCBs[!horizontal]);
    m_postFXShader->setInt("bloomBlur", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_dofBlurCBs[!horizontal]);
    m_postFXShader->setInt("sceneBlur", 2);

    m_postFXShader->setBool("bloom", bloom);
    m_postFXShader->setBool("useDOF", dof);

    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    m_postFXShader->setInt("depthTexture", 14);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::applyBloomBuffer(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffers[1]);
    shader->setInt("quadTexture", 1);
    shader->setInt("numTexVals", 4);
    CHECK_GL_ERROR_LINE_FILE();
}

void RenderToTexture::useBloomBlur(bool horizontal, bool first_iteration)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_bloomBlurFBOs[!horizontal]);
    m_blurShader->setBool("horizontal", horizontal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_iteration ? m_colorBuffers[1] : m_bloomBlurCBs[!horizontal]);
    m_blurShader->setInt("image", 0);
}

void RenderToTexture::useDOFBlur(bool horizontal, bool first_iteration)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_dofBlurFBOs[!horizontal]);
    m_blurShader->setBool("horizontal", horizontal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_iteration ? m_colorBuffers[2] : m_dofBlurCBs[!horizontal]);
    m_blurShader->setInt("image", 0);
}

void RenderToTexture::applyGAttribs(std::shared_ptr<vr::Shader> shader)
{
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[0]);
    shader->setInt("gPosition", 10);
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[1]);
    shader->setInt("gNormal", 11);
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[2]);
    shader->setInt("gAlbedoSpec", 12);
    CHECK_GL_ERROR_LINE_FILE();
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, m_gTextures[3]);
    shader->setInt("gAmbientShininess", 13);
    CHECK_GL_ERROR_LINE_FILE();
    
}
std::shared_ptr<vr::Shader> RenderToTexture::getPostFXShader(void) { return m_postFXShader; }
std::shared_ptr<vr::Shader> RenderToTexture::getBlurShader(void) { return m_blurShader; }
std::shared_ptr<vr::Shader> RenderToTexture::getDepthShader(void) { return m_depthShader; }
std::shared_ptr<vr::Shader> RenderToTexture::getGBufferShader(void) { return m_gShader; }