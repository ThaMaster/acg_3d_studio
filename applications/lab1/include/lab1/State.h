#ifndef STATE_H
#define STATE_H

#include <vr/Texture.h>
#include <vr/Shader.h>
#include <vr/Material.h>
#include <vr/Light.h>
#include <iostream>
class State 
{
    public:
        State(const std::string& name = "");

        ~State();

        std::shared_ptr<State> merge(std::shared_ptr<State> s);
        void apply();

        void setShader(std::shared_ptr<vr::Shader> s) { m_shader = s;}
        std::shared_ptr<vr::Shader> getShader(void) { return m_shader; }

        void setMaterial(std::shared_ptr<vr::Material> m) 
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
        std::shared_ptr<vr::Material> getMaterial(void) { return m_material; }

        void addTexture(std::shared_ptr<vr::Texture> t) 
        { 
            m_textures.push_back(t); 
        }

        void initTextures(void);
        unsigned int getTextureUnit(void) { return m_curr_texture_unit; }
        void incTextureUnit(void) { m_curr_texture_unit++; }

        void addLight(std::shared_ptr<vr::Light> light) { m_lights.push_back(light); }
        std::vector<std::shared_ptr<vr::Light>> getLights(void) { return m_lights; }

        void setEnableLight(std::shared_ptr<bool> b) { m_enableLight = b; }
        std::shared_ptr<bool> getEnableLight(void) { return m_enableLight; }

        void setCullFace(std::shared_ptr<bool> b) { m_cullFace = b; }
        std::shared_ptr<bool> getCullFace(void) { return m_cullFace; }

        std::string getStateName() { return m_stateName;}  

        void setShaderSwitch(bool b) { m_shaderSwitch = b; }
        bool getShaderSwitch() { return m_shaderSwitch; }   
   

    private:
        std::string m_stateName;
        std::shared_ptr<vr::Shader> m_shader;
        std::shared_ptr<vr::Material> m_material;
        std::vector<std::shared_ptr<vr::Light>> m_lights;
        std::vector<std::shared_ptr<vr::Texture>> m_textures;
        std::shared_ptr<bool> m_cullFace;
        std::shared_ptr<bool> m_enableLight;
        bool m_shaderSwitch;
        unsigned int m_curr_texture_unit = -1;
};

#endif