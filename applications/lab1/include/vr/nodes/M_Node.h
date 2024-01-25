#ifndef M_NODE_H
#define M_NODE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class NodeVisitor;

class M_Node
{
    public:
        
        M_Node(const std::string& name = "" ) : m_name(name) {}

        virtual void accept(NodeVisitor& v) = 0;

        std::string getName() { return m_name; }
    
    private:

        std::string m_name;
};

#endif