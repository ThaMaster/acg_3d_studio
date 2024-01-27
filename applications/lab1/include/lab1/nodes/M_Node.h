#ifndef M_NODE_H
#define M_NODE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "lab1/UpdateCallback.h"

class NodeVisitor;

class M_Node
{
    public:
        M_Node(const std::string& name = "" ) : m_name(name) {}
        virtual void accept(NodeVisitor& v) = 0;
        std::string getName() { return m_name; }

        void setUpdateCallback(UpdateCallback* callback);
        UpdateCallback* getUpdateCallback();

    private:
        std::string m_name;
        UpdateCallback *updateCallback;
};

#endif