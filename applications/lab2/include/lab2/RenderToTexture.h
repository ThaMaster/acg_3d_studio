#ifndef RENDERTOTEXTURE_H
#define RENDERTOTEXTURE_H

#include "vr/Shader.h"
#include "vr/Light.h"
#include <glad/glad.h>

class RenderToTexture 
{
    public:
        RenderToTexture();
        ~RenderToTexture();

        bool createRenderTarget(void);
        void switchToFramebuffer();
        void defaultBuffer(void);

        std::shared_ptr<vr::Shader> getDepthShader(void);

        void apply(std::shared_ptr<vr::Shader> s);

        void applyLightMatrix(std::shared_ptr<vr::Light> light, glm::vec2 nearFar);

    private:
        std::shared_ptr<vr::Shader> m_depthShader;
        GLuint m_frameBuffer;
        GLuint m_renderedTexture;
        GLuint m_depthTexture;
};

#endif