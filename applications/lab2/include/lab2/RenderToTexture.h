#ifndef RENDERTOTEXTURE_H
#define RENDERTOTEXTURE_H

#include <glad/glad.h>

class RenderToTexture 
{
    public:
        RenderToTexture();
        ~RenderToTexture();

        bool createRenderTarget(void);
        void renderTexture(std::shared_ptr<vr::Shader> shader);
        void defaultBuffer(void);

    private:
        GLuint m_frameBuffer;
        GLuint m_renderedTexture;
        GLuint m_depthRenderBuffer;
};

#endif