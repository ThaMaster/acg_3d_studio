#ifndef ROTATECALLBACK_H
#define ROTATECALLBACK_H

#include <glm/glm.hpp>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Geometry.h"

class RotateCallback : public UpdateCallback
{
    public:
        RotateCallback(glm::vec3 r) {}
        virtual void update(Transform& trans_node) override 
        {
            std::cout << "Updating a transform!" << std::endl;
        }
        
        virtual void update(Group& group_node) override {}
        virtual void update(Geometry& geo_node) override {}
};

#endif