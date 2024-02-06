#include "lab1/nodes/LOD.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

vr::BoundingBox LOD::calculateBoundingBox()
{
    vr::BoundingBox box;
    for(auto g : getObjects()) 
    {
        box.expand(g->calculateBoundingBox());
    }
    return box;
}

void LOD::accept(NodeVisitor& v)
{
    v.visit(*this);
}

Group* LOD::getObjectToRender(float distToCam)
{
    float interval = m_maxRenderDistance / m_objects.size();
    int index = distToCam/interval;
    if( index > m_objects.size()) {
        return nullptr;
    } else {
        return m_objects[index];
    }
}

float LOD::getDistanceToCamera(glm::vec3 camPos)
{
    glm::vec4 cameraPos = glm::vec4(camPos, 1.0f);
    float camDist = m_maxRenderDistance;
    if(m_objects.size() != 0)
    {
        glm::vec4 objectPosWorld = m_objects[0]->getObject2WorldMat() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        camDist = glm::distance(glm::vec4(objectPosWorld), cameraPos);
    }    
    return camDist;
}