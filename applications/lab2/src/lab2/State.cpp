#include "lab2/State.h"
#include <iostream>
#include "vr/Material.h"

std::string State::getStateName() { return m_stateName;}  

void State::setShader(std::shared_ptr<vr::Shader> s) { m_shader = s;}
std::shared_ptr<vr::Shader> State::getShader(void) { return m_shader; }

void State::addTexture(std::shared_ptr<vr::Texture> t) { m_textures.push_back(t); }
std::vector<std::shared_ptr<vr::Texture>> State::getTextures(void) { return m_textures; }

void State::addLight(std::shared_ptr<vr::Light> light) { m_lights.push_back(light); }
std::vector<std::shared_ptr<vr::Light>> State::getLights(void) { return m_lights; }
void State::setEnableLight(std::shared_ptr<bool> b) { m_enableLight = b; }
std::shared_ptr<bool> State::getEnableLight(void) { return m_enableLight; }

void State::setCullFace(std::shared_ptr<bool> b) { m_cullFace = b; }
std::shared_ptr<bool> State::getCullFace(void) { return m_cullFace; }

void State::setShaderSwitch(bool b) { m_shaderSwitch = b; }
bool State::getShaderSwitch() { return m_shaderSwitch; }   

/**
 * @brief Function for merging two states into one.
 *        If the given state is not instanciated, copy
 *        all the values to the merged state and return.
 *        Otherwise each value of the incoming state needs
 *        to be examined if it is set or not, if it is use
 *        it in the merged state, if not use the value in the
 *        current state instead.
 * 
 * @param s The state to merge with.
 * 
 * @return std::shared_ptr<State> 
 */
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

        if(m_textures.size() != 0)
            mergedState->setTextures(m_textures);

        return mergedState;
    }
    
    mergedState = std::shared_ptr<State>(new State("(" + m_stateName + " + " + s->getStateName() + ")" ));

    if(s->getMaterial())
        mergedState->setMaterial(s->getMaterial());
    else 
        mergedState->setMaterial(m_material);

    if(s->getTextures().size() != 0)
        mergedState->setTextures(s->getTextures());
    else
        mergedState->setTextures(m_textures);

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

/**
 * @brief Function for uploading all the state
 *        information to the shader that is present
 *        in the current state.
 */
void State::apply() 
{
    m_shader->use();

    if(m_material) {
        m_material->apply(m_shader);
    }

    if(m_textures.size() != 0)
        applyTextures(m_shader);

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

/**
 * @brief Function for setting a material. If the current
 *        state does not have a created material, set the
 *        material directly to the incoming material. Otherwise,
 *        just set all the material coefficients from the incomming
 *        material.
 * 
 * @param m The material to set to the current one.
 */
void State::setMaterial(std::shared_ptr<vr::Material> m) { m_material = m; }

std::shared_ptr<vr::Material> State::getMaterial(void) { return m_material; }

void State::setTextures(std::vector<std::shared_ptr<vr::Texture>> t) { m_textures = t; }

void State::applyTextures(std::shared_ptr<vr::Shader> shader)
{
    int i = 0;

    std::vector<int> slotActive;
    std::vector<int> slots; 
    slotActive.resize(m_textures.size());
    slots.resize(m_textures.size());
    for (int i = 0; i < m_textures.size(); i++)
    {
        slots[i] = i;
        slotActive[i] = m_textures[i] != nullptr;
        if (m_textures[i])
            m_textures[i]->bind();
    }

    shader->setIntVector("fragTexture.textures", slots);
    shader->setIntVector("fragTexture.activeTextures", slotActive);
}
