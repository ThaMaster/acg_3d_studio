#ifndef STATE_H
#define STATE_H

#include <vr/Texture.h>
#include <vr/Shader.h>
#include <vr/Material.h>
#include <vr/Light.h>

class State 
{
    public:
        State(const std::string& name = "");

        ~State();

        std::shared_ptr<State> merge(std::shared_ptr<State> s);
        void apply();

        void setShader(std::shared_ptr<vr::Shader> s)
        {
            m_shader = s;
        }

        std::shared_ptr<vr::Shader> getShader(void)
        {
            return m_shader;
        }

        void setMaterial(std::shared_ptr<vr::Material> m)
        {
            m_material = m;
        }

        std::shared_ptr<vr::Material> getMaterial(void)
        {
            return m_material;
        }

        void addLight(std::shared_ptr<vr::Light> light)
        {
            m_lights.push_back(light);
        }

        std::vector<std::shared_ptr<vr::Light>> getLights(void)
        {
            return m_lights;
        }

        void setCullFace(std::shared_ptr<bool> b)
        {
            m_cullFace = b;
        }

        std::shared_ptr<bool> getCullFace(void)
        {
            return m_cullFace;
        }

        std::string getStateName()
        {
            return m_stateName;
        }
        
    private:
        std::string m_stateName;
        std::shared_ptr<vr::Shader> m_shader;
        std::shared_ptr<vr::Material> m_material;
        std::vector<std::shared_ptr<vr::Texture>> m_texture;
        std::vector<std::shared_ptr<vr::Light>> m_lights;
        std::shared_ptr<bool> m_cullFace;
};

#endif