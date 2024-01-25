#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

class Visitor;

class Node 
{
    public:

        Node(const std::string& name = "") : m_name(name) {}

        virtual void accept(Visitor& v) = 0;

        std::string getName() { return m_name; }

    private:
        std::string m_name;
};

#endif