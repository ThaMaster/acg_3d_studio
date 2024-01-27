#ifndef GROUP_H
#define GROUP_H

#include "M_Node.h"

#include "lab1/visitors/NodeVisitor.h"

class Group : public M_Node
{
    public:

        Group(const std::string& name = "") : M_Node(name) {}

        void addChild(M_Node* n);

        void accept(NodeVisitor& v) override;

        std::vector<M_Node*> getChildren() { return m_children; }

    private:

        std::vector<M_Node*> m_children;
};

#endif