#include "lab1/nodes/LOD.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

vr::BoundingBox LOD::calculateBoundingBox(glm::mat4 m)
{
    for(auto g : getObjects()) 
    {
        m_boundingBox.expand(g->calculateBoundingBox(m));
    }
    return m_boundingBox;
}

void LOD::accept(NodeVisitor& v)
{
    v.visit(*this);
}

Group* LOD::getObjectToRender(float distToCam)
{
    float interval = m_maxRenderDistance / m_objects.size();
    int index = distToCam/interval;
    
    if( index >= m_objects.size()) {
        return m_objects.back();
    } else {
        return m_objects[index];
    }
}

float LOD::getDistanceToCamera(glm::vec3 camPos)
{
    return glm::distance(m_boundingBox.getCenter(), camPos);
}