#ifndef UPDATECALLBACK_H
#define UPDATECALLBACK_H

class Group;
class Transform;
class Geometry;
class LOD;

/**
 * @brief A base class that should be derived by all different
 *        UpdateCallback functions. They all need to implement 
 *        the update function in order to handle being called
 *        from all types of nodes.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class UpdateCallback
{
    public:
        virtual void update(Group&) = 0;
        virtual void update(Transform&) = 0;
        virtual void update(Geometry&) = 0;
        virtual void update(LOD&) = 0;
};

#endif