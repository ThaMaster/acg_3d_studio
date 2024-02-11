#ifndef TRANSLATECALLBACK_H
#define TRANSLATECALLBACK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Geometry.h"
#include "lab1/nodes/LOD.h"

class TranslateCallback : public UpdateCallback
{
    public:
        TranslateCallback(float s, glm::vec3 t) : speed(s), axis(t) {}
        
        virtual void update(Transform& trans_node) override 
        {
            trans_node.setTransformMat(glm::translate(trans_node.getTransfromMat(), axis*speed));
        }
        
        virtual void update(Group& group_node) override {}
        virtual void update(Geometry& geo_node) override {}
        virtual void update(LOD& lod_node) override {}
    private:
        glm::vec3 axis;
        float speed;
};

#endif