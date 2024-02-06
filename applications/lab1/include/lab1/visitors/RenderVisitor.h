#ifndef RENDERVISITOR_H
#define RENDERVISITOR_H

#include <stack>
#include "NodeVisitor.h"
#include "vr/Shader.h"
#include "lab1/State.h"

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

        void setCameraPos(glm::vec3 cameraPos) { m_camPos = cameraPos; }
        std::stack<std::shared_ptr<State>> m_stateStack;

        float calcCameraDistance(glm::vec3 position);

    private:
        glm::vec3 m_camPos;
        std::stack<glm::mat4> m_transformStack;
};

#endif