#ifndef M_NODE_H
#define M_NODE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include <vr/BoundingBox.h>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/State.h"

class NodeVisitor;

class M_Node
{
    public:
        M_Node(const std::string& name = "" ) : m_name(name), m_updateCallback(nullptr) {}
        virtual void accept(NodeVisitor& v) = 0;
        virtual vr::BoundingBox calculateBoundingBox() = 0;
        std::string getName() { return m_name; }

        void setUpdateCallback(UpdateCallback* callback)
        {
            m_updateCallback = std::shared_ptr<UpdateCallback>(callback);
        }

        std::shared_ptr<UpdateCallback> getUpdateCallback()
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
        std::shared_ptr<UpdateCallback> m_updateCallback;
};

#endif