#ifndef LIGHTCOLORCALLBACK_H
#define LIGHTCOLORCALLBACK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "lab1/callbacks/UpdateCallback.h"
#include "lab1/nodes/Transform.h"
#include "lab1/nodes/Group.h"
#include "lab1/nodes/Geometry.h"
#include "lab1/nodes/LOD.h"

class LightColorCallback : public UpdateCallback
{
    public:
        LightColorCallback(glm::vec3 s, glm::vec3 d) : m_changeSpec(s), m_changeDiff(d) {}
        
        virtual void update(Transform& trans_node) override 
        {
            if(trans_node.getState())
            {
                for(auto l : trans_node.getState()->getLights())
                {
                    if(l->getSpecular().r > 1 && m_changeSpec[0] > 0 || l->getSpecular().r < 0 && m_changeSpec[0] < 0)
                        m_changeSpec[0] *= -1;
                    if(l->getSpecular().g > 1 && m_changeSpec[1] > 0 || l->getSpecular().g < 0 && m_changeSpec[1] < 0)
                        m_changeSpec[1] *= -1;
                    if(l->getSpecular().b > 1 && m_changeSpec[2] > 0 || l->getSpecular().b < 0 && m_changeSpec[2] < 0)
                        m_changeSpec[2] *= -1;
                    
                    if(l->getDiffuse().r > 1 && m_changeDiff[0] > 0 || l->getDiffuse().r < 0 && m_changeDiff[0] < 0)
                        m_changeDiff[0] *= -1;
                    if(l->getDiffuse().g > 1 && m_changeDiff[1] > 0 || l->getDiffuse().g < 0 && m_changeDiff[1] < 0) 
                        m_changeDiff[1] *= -1;
                    if(l->getDiffuse().b > 1 && m_changeDiff[2] > 0 || l->getDiffuse().b < 0 && m_changeDiff[2] < 0)
                        m_changeDiff[2] *= -1;

                    l->setSpecular(l->getSpecular() + glm::vec4(m_changeSpec, 0.0));
                    l->setDiffuse(l->getDiffuse() + glm::vec4(m_changeDiff, 0.0));
                }
            }
        }
        
        virtual void update(Group& group_node) override 
        {
            if(group_node.getState())
            {
                for(auto l : group_node.getState()->getLights())
                {
                    if(l->getSpecular().r > 1 && m_changeSpec[0] > 0 || l->getSpecular().r < 0 && m_changeSpec[0] < 0)
                        m_changeSpec[0] *= -1;
                    if(l->getSpecular().g > 1 && m_changeSpec[1] > 0 || l->getSpecular().g < 0 && m_changeSpec[1] < 0)
                        m_changeSpec[1] *= -1;
                    if(l->getSpecular().b > 1 && m_changeSpec[2] > 0 || l->getSpecular().b < 0 && m_changeSpec[2] < 0)
                        m_changeSpec[2] *= -1;
                    
                    if(l->getDiffuse().r > 1 && m_changeDiff[0] > 0 || l->getDiffuse().r < 0 && m_changeDiff[0] < 0)
                        m_changeDiff[0] *= -1;
                    if(l->getDiffuse().g > 1 && m_changeDiff[1] > 0 || l->getDiffuse().g < 0 && m_changeDiff[1] < 0) 
                        m_changeDiff[1] *= -1;
                    if(l->getDiffuse().b > 1 && m_changeDiff[2] > 0 || l->getDiffuse().b < 0 && m_changeDiff[2] < 0)
                        m_changeDiff[2] *= -1;

                    l->setSpecular(l->getSpecular() + glm::vec4(m_changeSpec, 0.0));
                    l->setDiffuse(l->getDiffuse() + glm::vec4(m_changeDiff, 0.0));
                }
            }
        }
        virtual void update(Geometry& geo_node) override 
        {
            if(geo_node.getState())
            {
                for(auto l : geo_node.getState()->getLights())
                {
                    if(l->getSpecular().r > 1 && m_changeSpec[0] > 0 || l->getSpecular().r < 0 && m_changeSpec[0] < 0)
                        m_changeSpec[0] *= -1;
                    if(l->getSpecular().g > 1 && m_changeSpec[1] > 0 || l->getSpecular().g < 0 && m_changeSpec[1] < 0)
                        m_changeSpec[1] *= -1;
                    if(l->getSpecular().b > 1 && m_changeSpec[2] > 0 || l->getSpecular().b < 0 && m_changeSpec[2] < 0)
                        m_changeSpec[2] *= -1;
                    
                    if(l->getDiffuse().r > 1 && m_changeDiff[0] > 0 || l->getDiffuse().r < 0 && m_changeDiff[0] < 0)
                        m_changeDiff[0] *= -1;
                    if(l->getDiffuse().g > 1 && m_changeDiff[1] > 0 || l->getDiffuse().g < 0 && m_changeDiff[1] < 0) 
                        m_changeDiff[1] *= -1;
                    if(l->getDiffuse().b > 1 && m_changeDiff[2] > 0 || l->getDiffuse().b < 0 && m_changeDiff[2] < 0)
                        m_changeDiff[2] *= -1;

                    l->setSpecular(l->getSpecular() + glm::vec4(m_changeSpec, 0.0));
                    l->setDiffuse(l->getDiffuse() + glm::vec4(m_changeDiff, 0.0));
                }
            }
        }
        virtual void update(LOD& lod_node) override 
        {
            if(lod_node.getState())
            {
                for(auto l : lod_node.getState()->getLights())
                {
                    if(l->getSpecular().r > 1 && m_changeSpec[0] > 0 || l->getSpecular().r < 0 && m_changeSpec[0] < 0)
                        m_changeSpec[0] *= -1;
                    if(l->getSpecular().g > 1 && m_changeSpec[1] > 0 || l->getSpecular().g < 0 && m_changeSpec[1] < 0)
                        m_changeSpec[1] *= -1;
                    if(l->getSpecular().b > 1 && m_changeSpec[2] > 0 || l->getSpecular().b < 0 && m_changeSpec[2] < 0)
                        m_changeSpec[2] *= -1;
                    
                    if(l->getDiffuse().r > 1 && m_changeDiff[0] > 0 || l->getDiffuse().r < 0 && m_changeDiff[0] < 0)
                        m_changeDiff[0] *= -1;
                    if(l->getDiffuse().g > 1 && m_changeDiff[1] > 0 || l->getDiffuse().g < 0 && m_changeDiff[1] < 0) 
                        m_changeDiff[1] *= -1;
                    if(l->getDiffuse().b > 1 && m_changeDiff[2] > 0 || l->getDiffuse().b < 0 && m_changeDiff[2] < 0)
                        m_changeDiff[2] *= -1;

                    l->setSpecular(l->getSpecular() + glm::vec4(m_changeSpec, 0.0));
                    l->setDiffuse(l->getDiffuse() + glm::vec4(m_changeDiff, 0.0));
                }
            }
        }
    private:
        glm::vec3 m_changeSpec;
        glm::vec3 m_changeDiff;
};

#endif