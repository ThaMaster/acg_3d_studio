#ifndef GROUP_H
#define GROUP_H

#include "Node.h"

#include "vr/visitors/NodeVisitor.h"

class Group : public Node
{
    public:

        Group(const std::string& name = "") : Node(name) {}

        void addChild(Node* n);

        void accept(NodeVisitor& v) override;

        std::vector<Node*> getChildren() { return m_children; }

    private:

        std::vector<Node*> m_children;
};

#endif