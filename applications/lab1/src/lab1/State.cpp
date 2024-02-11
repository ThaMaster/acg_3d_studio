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

        if(m_material && m_material->getTextures().size() != 0)
            mergedState->setTextures(m_material->getTextures());
        
        return mergedState;
    }
    
    mergedState = std::shared_ptr<State>(new State("(" + m_stateName + " + " + s->getStateName() + ")" ));

    if(s->getMaterial()) {
        mergedState->setMaterial(s->getMaterial());
        if(s->getMaterial()->getTextures().size() != 0)
            mergedState->setTextures(s->getMaterial()->getTextures());
    } else {
        mergedState->setMaterial(m_material);
        if(m_material->getTextures().size() != 0)
            mergedState->setTextures(m_material->getTextures());
    }
    if(s->getShader()) 
        mergedState->setShader(s->getShader());
    else 
        mergedState->setShader(m_shader);
    
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
    }

    int unit = 0;
    for(auto t : m_textures)
    {
        m_material->setTexture(t, unit);
        unit++;
    }
}

void State::setMaterial(std::shared_ptr<vr::Material> m) 
{
    if(!m_material) {
        m_material = m;
    } else {
        m_material->setAmbient(m->getAmbient());
        m_material->setDiffuse(m->getDiffuse());
        m_material->setSpecular(m->getSpecular());
        m_material->setShininess(m->getShininess());
    }
}

void State::setTextures(std::vector<std::shared_ptr<vr::Texture>> textures) 
{ 
    int i = 0;
    for(auto t : textures)
    {
        m_material->setTexture(t, i);
        i++;
    }
}