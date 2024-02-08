#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include <vr/BoundingBox.h>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/State.h"

class NodeVisitor;

class Node
{
    public:
        Node(const std::string& name = "" ) : m_name(name) {}
        virtual void accept(NodeVisitor& v) = 0;
        virtual vr::BoundingBox calculateBoundingBox(glm::mat4 m) = 0;
        std::string getName() { return m_name; }

        void addUpdateCallback(UpdateCallback* callback)
        {
            m_updateCallback.push_back(std::shared_ptr<UpdateCallback>(callback));
        }

        std::vector<std::shared_ptr<UpdateCallback>> getUpdateCallbacks()
        {
            return m_updateCallback;
        }

        void setState(std::shared_ptr<State> s)
        {
            m_state = s;
        }

        std::shared_ptr<State> getState(void)
        {
            return m_state;
        }

    private:
        std::string m_name;
        std::shared_ptr<State> m_state;
        std::vector<std::shared_ptr<UpdateCallback>> m_updateCallback;
};

#endif