#include "lab1/visitors/UpdateVisitor.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Geometry.h"
#include "lab1/nodes/LOD.h"

void UpdateVisitor::visit(Group& group) 
{
    std::vector<std::shared_ptr<UpdateCallback>> callbackVector = group.getUpdateCallbacks();
    if(callbackVector.size() != 0)
    {
        for(auto uc : callbackVector )
            uc->update(group);
    }
    
    for (auto child : group.getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(Transform& trans) 
{
    std::vector<std::shared_ptr<UpdateCallback>> callbackVector = trans.getUpdateCallbacks();
    if(callbackVector.size() != 0)
    {
        for(auto uc : callbackVector )
            uc->update(trans);
    }

    for (auto child : trans.getChildren()) {
        child->accept(*this);
    }
}

void UpdateVisitor::visit(Geometry& geo) 
{
    std::vector<std::shared_ptr<UpdateCallback>> callbackVector = geo.getUpdateCallbacks();
    if(callbackVector.size() != 0)
    {
        for(auto uc : callbackVector )
            uc->update(geo);
    }
}

void UpdateVisitor::visit(LOD& lod) 
{
    std::vector<std::shared_ptr<UpdateCallback>> callbackVector = lod.getUpdateCallbacks();
    if(callbackVector.size() != 0)
    {
        for(auto uc : callbackVector )
            uc->update(lod);
    }
}