#include "lab1/State.h"
#include <iostream>
#include "vr/Material.h"

std::string State::getStateName() { return m_stateName;}  

void State::setShader(std::shared_ptr<vr::Shader> s) { m_shader = s;}
std::shared_ptr<vr::Shader> State::getShader(void) { return m_shader; }

void State::addTexture(std::shared_ptr<vr::Texture> t) { m_textures.push_back(t); }
unsigned int State::getTextureUnit(void) { return m_curr_texture_unit; }
void State::incTextureUnit(void) { m_curr_texture_unit++; }

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

/**
 * @brief Function for uploading all the state
 *        information to the shader that is present
 *        in the current state.
 */
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

/**
 * @brief Initializes the loaded textures with the correct
 *        unit indices. This will be useful when the program
 *        starts supporting multi-texturing.
 */
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

/**
 * @brief Function for setting a material. If the current
 *        state does not have a created material, set the
 *        material directly to the incoming material. Otherwise,
 *        just set all the material coefficients from the incomming
 *        material.
 * 
 * @param m The material to set to the current one.
 */
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

std::shared_ptr<vr::Material> State::getMaterial(void) { return m_material; }

void State::setTextures(std::vector<std::shared_ptr<vr::Texture>> textures) 
{ 
    int i = 0;
    for(auto t : textures)
    {
        m_material->setTexture(t, i);
        i++;
    }
}
