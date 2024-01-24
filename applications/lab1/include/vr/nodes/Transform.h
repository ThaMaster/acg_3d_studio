#ifndef TRANSFORM_H
#define TRANSFROM_H

#include "Group.h"

class Transform : public Group
{
    public:
        Transform(const std::string& name = "") : Group(name) {}
        void accept(NodeVisitor& v) override;
    
    private:

        glm::mat4 transformMat;
};

#endif