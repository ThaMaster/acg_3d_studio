#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>
#include "NodeVisitor.h"
#include "vr/Shader.h"

class RenderVisitor : public NodeVisitor
{
    public:
        void visit(Group& g) override;
        void visit(Transform& g) override;
        void visit(Geometry& g) override;
        void setShader(std::shared_ptr<vr::Shader> s)
        {
            m_shader = s;
        }
    private:
        std::shared_ptr<vr::Shader> m_shader;   
};

#endif