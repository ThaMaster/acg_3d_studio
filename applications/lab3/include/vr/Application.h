#pragma once

#include <memory>
#include "Scene.h"
#include "FPSCounter.h"
#include "Camera.h"

namespace vr {

  /**
  Class that holds a scene and a camera.
  */
  class Application
  {
  public:

    /**
    Constructor
    */
    Application(unsigned int width, unsigned int height);

    /**
    Destructor
    */
    ~Application();

    bool initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename);
    void initView();

    void setScreenSize(unsigned int width, unsigned int height);

    std::shared_ptr<Camera> getCamera();

    void reloadScene();
    void render(GLFWwindow* window);

    void update(GLFWwindow* window);

    void processInput(GLFWwindow* window);
    void lightInput(GLFWwindow* window, std::shared_ptr<vr::Light> light);
    void quadInput(GLFWwindow* window);

    void setClearColor(const glm::f32vec4& clearColor);
    
  private:
  
    BoundingBox m_bbox;
    std::shared_ptr<Scene> m_sceneRoot;
    std::shared_ptr<FPSCounter> m_fpsCounter;
    std::string m_loadedFilename, m_loadedVShader, m_loadedFShader;
    glm::uvec2 m_screenSize;
    glm::f32vec4 m_clearColor;
    float m_lastTime;
    bool m_spacePressed = false;
    bool m_lightSwitchPressed = false;
    bool m_lightTogglePressed = false;
    std::vector<bool> m_numPressed;
  };
}
