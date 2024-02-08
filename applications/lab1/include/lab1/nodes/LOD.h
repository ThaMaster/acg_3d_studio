#ifndef LOD_H
#define LOD_H

#include "Group.h"

class LOD : public Node
{
    public:
        LOD(const std::string& name = "") : Node(name) {}

        void accept(NodeVisitor& v) override;

        vr::BoundingBox calculateBoundingBox(glm::mat4 m) override;

        std::vector<Group*> getObjects(void) { return m_objects; }
        void addObject(Group* obj) { m_objects.push_back(obj); }
        Group* getObjectToRender(float distToCam);
        
        void setMaxRenderDistance(float dist) { m_maxRenderDistance = dist; }
        float getMaxRenderDistance(void) { return m_maxRenderDistance; }

        float getDistanceToCamera(glm::vec3 camPos);

        bool firstTime = false;
    private:
        std::vector<Group*> m_objects;
        float m_maxRenderDistance;
        vr::BoundingBox m_boundingBox;
};

#endif