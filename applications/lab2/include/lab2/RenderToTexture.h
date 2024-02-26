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

        bool addRenderTarget();
        void switchToDepthbuffer(void);
        void defaultBuffer();

        std::shared_ptr<vr::Shader> getDepthShader(void);

        void apply(std::shared_ptr<vr::Shader> s, unsigned int unit);

        void applyLightMatrix(glm::mat4 lms);

    private:
        std::shared_ptr<vr::Shader> m_depthShader;
        std::vector<GLuint> m_depthTextures;
        GLuint m_frameBuffer;
        GLuint m_renderedTexture;
        int m_numRenderTargets = 0;
};

#endif