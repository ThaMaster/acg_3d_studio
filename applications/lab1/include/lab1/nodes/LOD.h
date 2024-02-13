#ifndef LOD_H
#define LOD_H

#include "Group.h"

/**
 * @brief A node that implements the Level of Detail (LOD)
 *        functionality. Depending on the set max distance,
 *        the node can supply a specific geometry depending
 *        on the given distance.
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 */
class LOD : public Node
{
    public:
        LOD(const std::string& name = "") : Node(name) {}
        ~LOD(){}
        void accept(NodeVisitor& v) override;

        vr::BoundingBox calculateBoundingBox(glm::mat4 m) override;

        std::vector<Group*> getObjects(void);
        void addObject(Group* obj);
        Group* getObjectToRender(float distToCam);
        
        void setMaxRenderDistance(float dist);
        float getMaxRenderDistance(void);

        float getDistanceToCamera(glm::vec3 camPos);

    private:
        std::vector<Group*> m_objects;
        float m_maxRenderDistance;
        vr::BoundingBox m_boundingBox;
};

#endif