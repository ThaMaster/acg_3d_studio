#ifndef RENDERTOTEXTURE_H
#define RENDERTOTEXTURE_H

#include "vr/Shader.h"

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
        glm::mat4 calcLightMatrix(glm::vec4 l_pos, glm::vec2 nearFar);

        void apply(std::shared_ptr<vr::Shader> s);

    private:
        std::shared_ptr<vr::Shader> m_depthShader;
        GLuint m_frameBuffer;
        GLuint m_renderedTexture;
        GLuint m_depthTexture;
};

#endif