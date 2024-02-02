#include "lab1/State.h"

State::State(const std::string& name) : m_stateName(name) 
{

}

State::~State() {}

void State::apply(State& s) 
{
    if(m_shader)
        s.set_shader(m_shader);

    if(m_material)
        s.set_material(m_material);

    if(m_lights.size() != 0)
    {

    }

    if(m_texture.size() != 0)
    {

    }
}