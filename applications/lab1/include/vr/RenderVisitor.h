#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>
#include "NodeVisitor.h"
#include "Group.h"
#include "Transform.h"
#include "Geometry.h"

class RenderVisitor : public NodeVisitor
{
    public:
        
        void visit(Group& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            for (auto child : g.getChildren()) {
                child->accept(*this);
            }
        }

        void visit(Transform& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            for (auto child : g.getChildren()) {
                child->accept(*this);
            }
        }

        void visit(Geometry& g) override
        {
            std::cerr << "Visit: " << g.getName() << std::endl;
            g.render();
        }

        void push_trans_mat(glm::mat4 t)
        {
            m_trans_stack.push(t);
        }

        void pop_trans_mat()
        {
            m_trans_stack.pop();
        }

        glm::mat4 top_trans_mat(glm::mat4 t)
        {
            return m_trans_stack.top();
        }
    
    private:

        std::stack<glm::mat4> m_trans_stack;
};

#endif