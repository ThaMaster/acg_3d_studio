#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>
#include "NodeVisitor.h"
#include "vr/Shader.h"
#include "lab3/State.h"
#include "lab3/Quad.h"
#include "lab3/RenderToTexture.h"
#include "vr/Camera.h"
/**
 * @brief The RenderVisitor will traverse the scene graph and
 *        gather information such as transformation and states
 *        and will supply this information to the geometry when
 *        calling the draw function. This enables different parts
 *        of the scene graph to be rendered differently.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version.
 *      2024-03-19: v1.5, second version, implemented support for deferred rendering.
 */
class RenderVisitor : public NodeVisitor
{
    public:
        RenderVisitor()
        {
            m_transformStack.push(glm::mat4());
        }

        void visit(Group& g) override;
        void visit(Transform& g) override;
        void visit(Geometry& g) override;
        void visit(LOD& g) override;

        std::stack<std::shared_ptr<State>> m_stateStack;
        void setCamera(std::shared_ptr<vr::Camera> c);
        std::shared_ptr<vr::Camera> getCamera(void);

        void setRTT(std::shared_ptr<RenderToTexture> rtt);
        std::shared_ptr<RenderToTexture> getRTT(void);

        void setDepthPass(bool b);
        bool getDepthPass(void);

        void setGBufferPass(bool b);
        bool getGBufferPass(void);

        void setUseShadowMap(bool b);
        bool getUseShadowMap(void);

        void setCurrentLight(std::shared_ptr<vr::Light> light);
        void setLightShader(std::shared_ptr<vr::Shader> s);
        
    private:
        std::shared_ptr<vr::Shader> m_lightShader;
        bool m_depthPass = false;
        bool m_gBufferPass = false;
        bool m_useShadowMap = false;
        std::shared_ptr<vr::Camera> m_camera;
        std::stack<glm::mat4> m_transformStack;
        std::vector<std::vector<glm::mat4>> m_lightMatrices;
        std::shared_ptr<RenderToTexture> m_rtt;
        std::shared_ptr<vr::Light> m_currLight;
};

#endif