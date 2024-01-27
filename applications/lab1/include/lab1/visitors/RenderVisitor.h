#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>
#include "NodeVisitor.h"
#include "vr/Shader.h"

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

        void setShader(std::shared_ptr<vr::Shader> s)
        {
            m_shader = s;
        }

        std::shared_ptr<vr::Shader> getShader(void)
        {
            return m_shader;
        }
    private:
        std::stack<glm::mat4> m_transformStack;
        std::shared_ptr<vr::Shader> m_shader;   
};

#endif