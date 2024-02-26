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
        void switchToDepthbuffer(void);
        void defaultBuffer();

        std::shared_ptr<vr::Shader> getDepthShader(void);

        void apply(std::shared_ptr<vr::Shader> s);

        void applyLightMatrix(glm::mat4 lm);

    private:
        std::shared_ptr<vr::Shader> m_depthShader;
        GLuint m_frameBuffer;
        GLuint m_renderedTexture;
        GLuint m_depthTexture;
};

#endif