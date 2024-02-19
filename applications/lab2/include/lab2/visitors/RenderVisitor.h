#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>
#include "NodeVisitor.h"
#include "vr/Shader.h"
#include "lab2/State.h"
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

    private:
        std::shared_ptr<vr::Camera> m_camera;
        std::stack<glm::mat4> m_transformStack;
};

#endif