#ifndef STATE_H
#define STATE_H

#include <vr/Texture.h>
#include <vr/Shader.h>
#include <vr/Material.h>
#include <vr/Light.h>
#include <iostream>

/**
 * @brief A state represents the current context of which
 *        is to be used when going through the scene graph.
 *        The state will hold information such as what shaders
 *        should be used, textures, material coefficients and
 *        more. Since all nodes in the graph does not potentially
 *        need to have all the values inside the state set, all
 *        attributes uses the shared_ptr datatype. 
 * 
 * @author Christoffer Nordlander (c20cnr@cs.umu.se)
 * 
 * Version information:
 *      2024-02-12: v1.0, first version. 
 *      2024-03-01: v1.1, second version, can now handle multiple textures.
 *      2024-03-19: v1.5, third version, corrected the texture units when binding
 *                        textures and added support for procedural textures.
 */
class State 
{
    public:
        State(const std::string& name) : m_stateName(name) {}
        ~State() {}

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
        std::shared_ptr<State> merge(std::shared_ptr<State> s);

        /**
         * @brief Function for uploading all the state
         *        information to the shader that is present
         *        in the current state.
         */
        void apply(bool b);

        void applyLights(std::shared_ptr<vr::Shader> shader);

        /**
         * @brief Initializes the loaded textures with the correct
         *        unit indices. This will be useful when the program
         *        starts supporting multi-texturing.
         */
        void initTextures(void);

        void addTexture(std::shared_ptr<vr::Texture> t);
        void setTextures(std::vector<std::shared_ptr<vr::Texture>> textures);

        /**
         * @brief Applies all the textures on the current state to
         *        the specified shader.
         * 
         * @param shader The shader to apply the textures to.
         */
        void applyTextures(std::shared_ptr<vr::Shader> shader);
        std::vector<std::shared_ptr<vr::Texture>> getTextures(void);

        void addLight(std::shared_ptr<vr::Light> light);
        std::vector<std::shared_ptr<vr::Light>> getLights(void);

        void setEnableLight(std::shared_ptr<bool> b);
        std::shared_ptr<bool> getEnableLight(void);

        void setCullFace(std::shared_ptr<bool> b);
        std::shared_ptr<bool> getCullFace(void);

        void setShader(std::shared_ptr<vr::Shader> s);
        std::shared_ptr<vr::Shader> getShader(void);
        
        void setMaterial(std::shared_ptr<vr::Material> m);
        std::shared_ptr<vr::Material> getMaterial(void);

        std::string getStateName(); 

        void setShaderSwitch(bool b);
        bool getShaderSwitch();
   
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