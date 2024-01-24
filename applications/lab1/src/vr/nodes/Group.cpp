#include "vr/nodes/Group.h"
#include "vr/visitors/NodeVisitor.h"

void Group::addChild(Node* n)
{
    m_children.push_back(n);
}

void Group::accept(NodeVisitor& v)
{
    std::cerr << "Accept: " << getName() << std::endl;
    v.visit(*this);
}