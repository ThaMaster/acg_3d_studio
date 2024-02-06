#ifndef ROTATECALLBACK_H
#define ROTATECALLBACK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Geometry.h"
#include "lab1/nodes/LOD.h"

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