#ifndef TRANSLATECALLBACK_H
#define TRANSLATECALLBACK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab2/callbacks/UpdateCallback.h"
#include "lab2/nodes/Transform.h"
#include "lab2/nodes/Group.h"
#include "lab2/nodes/Geometry.h"
#include "lab2/nodes/LOD.h"

/**
 * @brief Class for changing the transform nodes transformation matrix by
 *        using the given values to translate the matrix for the transform
 *        node.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
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