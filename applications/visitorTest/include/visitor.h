#ifndef VISITOR_H
#define VISITOR_H

#include <iostream>
#include <vector>

class Group;
class Transform;
class Geometry;

class Visitor
{
    public:
        virtual void visit(Group&) = 0;
        virtual void visit(Transform&) = 0;
        virtual void visit(Geometry&) = 0;
};

#endif