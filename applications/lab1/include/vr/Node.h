#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

class NodeVisitor;

class Node 
{
    public:

        Node(const std::string& name = "") : m_name(name) {}

        virtual void accept(NodeVisitor& v) = 0;

        std::string getName() { return m_name; }

    private:
        std::string m_name;
};

#endif