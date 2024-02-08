#ifndef TRANSFORM_H
#define TRANSFROM_H

#include "Group.h"

class Transform : public Group
{
    public:
        Transform(const std::string& name = "") : Group(name) {}
        void accept(NodeVisitor& v) override;
        vr::BoundingBox calculateBoundingBox(glm::mat4 m) override;


        void setTransformMat(glm::mat4 m)
        {
            m_transformMat = m;
        }
        
        glm::mat4 getTransfromMat(void)
        {
            return m_transformMat;
        }
        
    private:

        glm::mat4 m_transformMat;
};

#endif