#ifndef NODEVISITOR_H
#define NODEVISITOR_H

#include <memory>
#include <iostream>
#include <vector>
#include <stack>
#include <glm/glm.hpp>

class Group;
class Transform;
class Geometry;
class LOD;

/**
 * @brief Base class that all different types of 
 *        NodeVisitors should derive from. They all
 *        must implement the visit function with the
 *        wanted functionality.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class NodeVisitor
{
    public:
        virtual void visit(Group&) = 0;
        virtual void visit(Transform&) = 0;
        virtual void visit(Geometry&) = 0;
        virtual void visit(LOD&) = 0;

    private:
        
};

#endif