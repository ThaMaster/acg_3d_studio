#ifndef RENDERTOTEXTURE_H
#define RENDERTOTEXTURE_H

#include "vr/Shader.h"
#include "vr/Light.h"
#include <glad/glad.h>

/**
 * @brief A class for handling rendering to a texture.
 *        As of writing this code, this class only handles
 *        rendering depth maps to textures but it can be
 *        further developed to render additional information
 *        to other textures. The class can switch between multiple
 *        framebuffers for deferred rendering.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-03-01: v1.0, first version. 
 *      2024-03-19: v1.5, second version, implemented the support for deferred
 *                        shading.
 */
class RenderToTexture 
{
    public:
        RenderToTexture();
        ~RenderToTexture();

        /**
         * @brief Adds a new depth texture to the texture vector in the
         *        class. The class will set a texture unit to the texture
         *        and increase the index such that if another texture
         *        were to be added, they wont collide.
         */
        void addDepthMap();

        /**
         * @brief Adds a new depth cube map to the cubemap vector in the
         *        class. The class will set a texture unit to the cubemap
         *        and increase the index such that if another texture
         *        were to be added, they wont collide.
         */
        void addDepthCubeMap();

        /**
         * @brief Function for switching to the depth frame buffer and
         *        bind a specified texture unit, in this case a texture 
         *        unit correlating a depth texture. This function should
         *        be used for each directional light and the unit specifies 
         *        the current light to render to.
         * 
         * @param unit The texture unit of the depth texture to bind. 
         */
        void switchToDepthTexture(unsigned int unit);

        /**
         * @brief Function for switching to the depth frame buffer and
         *        bind a specified texture unit, in this case a texture 
         *        unit correlating a depth cubemap. This function should
         *        be used for each point light and the unit specifies the
         *        current light to render to.
         * 
         * @param unit The texture unit of the the depth cubemap to bind.
         */
        void switchToDepthCubeMap(unsigned int unit);

        /**
         * @brief Function for switching to the default frame buffer that
         *        is used to render the final scene. This should be used
         *        right before rendering the actual scene.
         */
        void defaultBuffer();

        /**
         * @brief Applies all the added depth textures and depth maps
         *        to a specified shader. The shader will need to be 
         *        able to handle both 2D depth textures and 3D depth
         *        cubemaps.
         * 
         * @param s The shader where the depth map should be applied to.
         */
        void applyDepthMaps(std::shared_ptr<vr::Shader> s);

        /**
         * @brief Applies the necessary data to the depth shader that
         *        is loaded to into this class used to create the 
         *        depth textures. Depending on the light position
         *        it will apply different kinds of data since directional
         *        and point lights need different information.
         * 
         * @param lms       The light space matrices for the light.
         * @param l_pos     The position of the light.
         * @param farPlane  The far plane of the view frustum.
         */
        void applyDepthData(std::vector<glm::mat4> lms, glm::vec4 l_pos, float farPlane);

        std::shared_ptr<vr::Shader> getDepthShader(void);
        std::shared_ptr<vr::Shader> getGBufferShader(void);
        std::shared_ptr<vr::Shader> getBlurShader(void);
        std::shared_ptr<vr::Shader> getPostFXShader(void);
        
        void initFrameBuffer(void);
        void initBlurBuffers(void);

        void initGBufferAttribs(void);
        
        void applyGAttribs(std::shared_ptr<vr::Shader> shader);
        void applyDepthTexture(std::shared_ptr<vr::Shader> shader, int texUnit, int texPos);
        void applyDepthCubeMap(std::shared_ptr<vr::Shader> shader, int texUnit, int texPos);
        void applyPositionTexture(std::shared_ptr<vr::Shader> shader);
        void applyNormalTexture(std::shared_ptr<vr::Shader> shader);
        void applyDiffuseTexture(std::shared_ptr<vr::Shader> shader);
        void applySpecularTexture(std::shared_ptr<vr::Shader> shader);
        void applyTextureColor(std::shared_ptr<vr::Shader> shader);
        void applyCamDepth(std::shared_ptr<vr::Shader> shader);
        void applyLightDepth(std::shared_ptr<vr::Shader> shader, int lightIdx, glm::vec4 l_pos, float farPlane);

        void usePostFXShader(bool bloom, bool dof, bool horizontal);
        void applyBloomBuffer(std::shared_ptr<vr::Shader> shader);
        
        void useBloomBlur(bool horizontal, bool first_iteration);
        void useDOFBlur(bool horizontal, bool first_iteration);

        void bindFB(void);
        void bindGBuffer(void);

    private:
        std::shared_ptr<vr::Shader> m_depthShader;
        std::shared_ptr<vr::Shader> m_postFXShader;
        std::shared_ptr<vr::Shader> m_blurShader;

        std::vector<GLuint> m_depthTextures;
        std::vector<GLuint> m_depthCubeMaps;

        GLuint m_frameBuffer;
        std::vector<GLuint> m_colorBuffers;

        std::vector<GLuint> m_bloomBlurFBOs;
        std::vector<GLuint> m_bloomBlurCBs;

        std::vector<GLuint> m_dofBlurFBOs;
        std::vector<GLuint> m_dofBlurCBs;

        GLuint m_depthBuffer;
        int m_num_depth_components = 0;

        std::shared_ptr<vr::Shader> m_gShader;
        GLuint m_gBuffer;
        std::vector<GLuint> m_gTextures;
        GLuint m_depthTexture;
};

#endif