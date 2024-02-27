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

        bool addDepthMap();
        bool addDepthCubemap();

        void switchToDepthTexture(unsigned int unit);
        void switchToDepthCubemap(unsigned int unit);
        void defaultBuffer();

        std::shared_ptr<vr::Shader> getDepthShader(void);

        void apply(std::shared_ptr<vr::Shader> s);

        void applyLightMatrix(glm::mat4 lms);

    private:
        std::shared_ptr<vr::Shader> m_depthShader;
        std::vector<GLuint> m_depthTextures;
        std::vector<GLuint> m_depthCubeMaps;
        GLuint m_depthBuffer;
        int m_numRenderTargets = 0;
};

#endif