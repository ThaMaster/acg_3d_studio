#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Group.h"

class Transform : public Group
{
    public:

        Transform(const std::string& name = "") : Group(name) {}

        void accept(Visitor& v) override
        {
            // push transform
            Group::accept(v);
            // pop transform
        }
};

#endif