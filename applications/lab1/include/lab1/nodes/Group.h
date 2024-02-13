#ifndef GROUP_H
#define GROUP_H

#include "Node.h"

#include "lab1/visitors/NodeVisitor.h"

/**
 * @brief The group node includes a list of children 
 *        node which can be of any size. The list uses
 *        the base class of Node which enables all nodes
 *        that are derived from that class to be present 
 *        in the list. 
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version.  
 */
class Group : public Node
{
    public:

        Group(const std::string& name = "") : Node(name) {}
        ~Group() {}
        
        void addChild(Node* n);

        void accept(NodeVisitor& v) override;
        vr::BoundingBox calculateBoundingBox(glm::mat4 m) override;

        std::vector<Node*> getChildren();

    private:
        std::vector<Node*> m_children;
};

#endif