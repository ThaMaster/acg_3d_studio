#ifndef UPDATECALLBACK_H
#define UPDATECALLBACK_H

class Group;
class Transform;
class Geometry;

class UpdateCallback
{
    public:
        virtual void update(Group&) = 0;
        virtual void update(Transform&) = 0;
        virtual void update(Geometry&) = 0;
};

#endif