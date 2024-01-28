#ifndef STATE_H
#define STATE_H

#include <vr/Texture.h>
#include <vr/Shader.h>
#include <vr/Material.h>

class State 
{
    public:
        State();
        void apply(State& s);

    private:
        std::shared_ptr<vr::Shader> m_shader;
        std::shared_ptr<vr::Material> m_material;
        std::vector<std::shared_ptr<vr::Texture>> m_texture;
};

#endif