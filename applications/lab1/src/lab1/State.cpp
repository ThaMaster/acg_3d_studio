#include "lab1/State.h"
#include <iostream>
#include "vr/Material.h"

State::State(const std::string& name) : m_stateName(name) {}
State::~State() {}

std::shared_ptr<State> State::merge(std::shared_ptr<State> s)
{
    std::shared_ptr<State> mergedState;
    if(!s) {
        mergedState = std::shared_ptr<State>(new State(m_stateName));
        mergedState->setMaterial(m_material);
        mergedState->setShader(m_shader);
        mergedState->setCullFace(m_cullFace);
        mergedState->setEnableLight(m_enableLight);
        for(auto l : m_lights)
            mergedState->addLight(l);
        return mergedState;
    }
    mergedState = std::shared_ptr<State>(new State("(" + m_stateName + " + " + s->getStateName() + ")" ));

    if(m_material) 
        mergedState->setMaterial(m_material);
    else 
        mergedState->setMaterial(s->getMaterial());

    if(s->getShader()) {
        mergedState->setShader(s->getShader());
    } else {
        mergedState->setShader(m_shader);
    }
    
    for(auto l : m_lights)
        mergedState->addLight(l);

    for(auto l : s->getLights())
        mergedState->addLight(l);
    
    if(s->getCullFace()) 
        mergedState->setCullFace(s->getCullFace());
    else 
        mergedState->setCullFace(m_cullFace);

    if(s->getEnableLight()) 
        mergedState->setEnableLight(s->getEnableLight());
    else 
        mergedState->setEnableLight(m_enableLight);

    return mergedState;
}

void State::apply() 
{
    m_shader->use();

    if(m_material)
        m_material->apply(m_shader);

    m_shader->setInt("numberOfLights", (GLint)m_lights.size());

    size_t i = 0;
    if(m_enableLight) {
        if(m_lights.size() != 0 && *m_enableLight)
        {
            for(auto l : m_lights)
            {
                l->apply(m_shader, i);
                i++;
            }
        }
    }

    if(m_cullFace) {
        if(*m_cullFace)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }
}

void State::initTextures(void)
{
    if(!m_material) {
        m_material = std::make_shared<vr::Material>();
        m_material->setDiffuse(glm::vec4(1,1,1,1));
        m_material->setSpecular(glm::vec4(1,1,1,1));
        m_material->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0));
    }

    int unit = 0;
    for(auto t : m_textures)
    {
        std::cout << "unit nr: " << unit << std::endl;
        m_material->setTexture(t, unit);
        unit++;
    }
}