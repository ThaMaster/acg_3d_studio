#ifndef GROUP_H
#define GROUP_H

#include "Node.h"

#include "lab1/visitors/NodeVisitor.h"

class Group : public Node
{
    public:

        Group(const std::string& name = "") : Node(name) {}

        void addChild(Node* n);

        void accept(NodeVisitor& v) override;
        vr::BoundingBox calculateBoundingBox() override;

        std::vector<Node*> getChildren() { return m_children; }

        glm::mat4& getObject2WorldMat() { return m_object2world; }
        void setObject2WorldMat(glm::mat4& m) { m_object2world = m; }

    private:
        glm::mat4 m_object2world;
        std::vector<Node*> m_children;
};

#endif