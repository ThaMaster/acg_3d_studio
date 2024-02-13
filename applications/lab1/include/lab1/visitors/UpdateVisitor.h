#ifndef UPDATEVISITOR_H
#define UPDATEVISITOR_H

#include "NodeVisitor.h"
/**
 * @brief The UpdateVisitor will traverse through the
 *        scene graph and check if nodes contain 
 *        UpdateCallbacks, if they do this visitor
 *        will execute the update function from the
 *        UpdateCallback.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class UpdateVisitor : public NodeVisitor
{    
    public:
        UpdateVisitor() {}

        void visit(Group& g) override;
        void visit(Transform& g) override;
        void visit(Geometry& g) override;  
        void visit(LOD& g) override;  

    private:
};

#endif