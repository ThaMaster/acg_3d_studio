#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include <vr/BoundingBox.h>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/State.h"

class NodeVisitor;

/**
 * @brief A base class for all the nodes in the scene graph to
 *        derive from. Methods such as accepting NodeVisitors,
 *        handle UpdateCallbacks should be present here. Further
 *        functoinality should be implemented in the actual node.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class Node
{
    public:

        Node(const std::string& name = "" ) : m_name(name) {}

        virtual void accept(NodeVisitor& v) = 0;

        std::string getName() { return m_name; }

        void addUpdateCallback(UpdateCallback* callback) { m_updateCallback.push_back(std::shared_ptr<UpdateCallback>(callback)); }
        std::vector<std::shared_ptr<UpdateCallback>> getUpdateCallbacks() { return m_updateCallback; }

        void setState(std::shared_ptr<State> s){ m_state = s; }
        std::shared_ptr<State> getState(void) { return m_state;}

        virtual vr::BoundingBox calculateBoundingBox(glm::mat4 m) = 0;

    private:
        std::string m_name;
        std::shared_ptr<State> m_state;
        std::vector<std::shared_ptr<UpdateCallback>> m_updateCallback;
};

#endif