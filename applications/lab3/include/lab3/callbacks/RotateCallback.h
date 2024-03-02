#ifndef ROTATECALLBACK_H
#define ROTATECALLBACK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab3/callbacks/UpdateCallback.h"
#include "lab3/nodes/Transform.h"
#include "lab3/nodes/Group.h"
#include "lab3/nodes/Geometry.h"
#include "lab3/nodes/LOD.h"

/**
 * @brief Class for changing the transform nodes transformation matrix by
 *        using the given values to rotate the matrix of a transform node. 
 *        It can be used with the TranslateCallback function to make the 
 *        object move in a circle or other types of movement.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class RotateCallback : public UpdateCallback
{
    public:
        RotateCallback(float d, glm::vec3 r) : angleInDegrees(d), axis(r) {}
        
        virtual void update(Transform& trans_node) override 
        {
            trans_node.setTransformMat(glm::rotate(trans_node.getTransfromMat(), glm::radians(angleInDegrees), axis));
        }
        
        virtual void update(Group& group_node) override {}
        virtual void update(Geometry& geo_node) override {}
        virtual void update(LOD& lod_node) override {}
    private:
        glm::vec3 axis;
        float angleInDegrees;
};

#endif