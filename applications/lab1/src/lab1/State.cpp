#include "lab1/State.h"
#include <iostream>
State::State(const std::string& name) : m_stateName(name) {}
State::~State() {}

std::shared_ptr<State> State::merge(std::shared_ptr<State> s)
{
    std::shared_ptr<State> mergedState = std::shared_ptr<State>(new State("MERGED STATE"));
    if(!s) {
        mergedState->setMaterial(m_material);
        mergedState->setShader(m_shader);
        mergedState->setCullFace(m_cullFace);
        for(auto l : m_lights)
            mergedState->addLight(l);

        return mergedState;
    }

    if(s->getMaterial())
        mergedState->setMaterial(s->getMaterial());

    if(s->getShader())
        mergedState->setShader(s->getShader());
    
    if(s->getLights().size() != 0)
    {
        for(auto l : s->getLights())
        {
            mergedState->addLight(l);
        }
    }

    if(s->getCullFace())
        mergedState->setCullFace(s->getCullFace());

    return mergedState;
}

void State::apply(std::shared_ptr<vr::Shader>& shader) 
{
    if(m_material)
    {
        m_material->apply(shader);
    }

    shader->setInt("numberOfLights", (GLint)m_lights.size());

    // Apply lightsources
    size_t i = 0;
    if(m_lights.size() != 0)
    {
        for(auto l : m_lights)
        {
            l->apply(shader, i);
        }
    }

    if(m_texture.size() != 0)
    {
    }
}