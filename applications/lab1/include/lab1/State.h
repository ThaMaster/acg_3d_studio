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
        
        void apply(State& s);

        void set_shader(std::shared_ptr<vr::Shader> s)
        {
            m_shader = s;
        }

        void set_material(std::shared_ptr<vr::Material> m)
        {
            m_material = m;
        }

    private:
        std::string m_stateName;
        std::shared_ptr<vr::Shader> m_shader;
        std::shared_ptr<vr::Material> m_material;
        std::vector<std::shared_ptr<vr::Texture>> m_texture;
        std::vector<std::shared_ptr<vr::Light>> m_lights;
};

#endif