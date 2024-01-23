#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vr/Text.h>

namespace vr
{
  /// Class that prints the framerate to the screen
  class FPSCounter
  {
  public:

    FPSCounter();

    void render(GLFWwindow* window);
    void setColor(const glm::vec4& color);
    void setFontScale(float scale);

  private:
    void update();

    double m_lastTime;
    unsigned int m_numFrames;
    double m_fps;

    vr::Text m_text;
  };

}
