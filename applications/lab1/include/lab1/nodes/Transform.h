#ifndef TRANSFORM_H
#define TRANSFROM_H
#pragma once

#include "Group.h"

/**
 * @brief A transform node works the same way as group
 *        nodes, but does include a transformation matrix
 *        that should transform each of the nodes child nodes.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class Transform : public Group
{
    public:
        Transform(const std::string& name = "") : Group(name) {}
        ~Transform(){}
        
        void accept(NodeVisitor& v) override;
        vr::BoundingBox calculateBoundingBox(glm::mat4 m) override;
        void setTransformMat(glm::mat4 m);
        glm::mat4 getTransfromMat(void);
        
    private:

        glm::mat4 m_transformMat;
};

#endif