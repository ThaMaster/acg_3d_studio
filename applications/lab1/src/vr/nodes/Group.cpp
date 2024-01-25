#include "vr/nodes/Group.h"

void Group::addChild(Node* n)
{
    m_children.push_back(n);
}

void Group::accept(NodeVisitor& v)
{
    std::cerr << "Accept: " << getName() << std::endl;
    v.visit(*this);
}