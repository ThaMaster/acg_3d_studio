#include <vr/FPSCounter.h>
#include <sstream>

#include <iomanip>

using namespace vr;

FPSCounter::FPSCounter() : m_lastTime(0), m_numFrames(0), m_fps(0)
{
  m_text.setPosition(glm::vec2(0.0, 0.97));
  m_text.setScale(0.5f);
}


void FPSCounter::setColor(const glm::vec4& color)
{
  m_text.setColor(color);
}

void FPSCounter::render(GLFWwindow* window)
{
  update();

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  std::ostringstream str;
  str << "FPS: " << std::fixed << std::setprecision(1) << m_fps;
  m_text.setText(str.str());
  m_text.render(width, height);
}
void FPSCounter::setFontScale(float scale)
{
  m_text.setScale(scale);
}


void FPSCounter::update()
{
  m_numFrames++;
  double now = glfwGetTime();
  double delta_t = now - m_lastTime;
  if (delta_t > 50e-3) {
    m_fps = m_numFrames / delta_t;

    m_numFrames = 0;
    m_lastTime = now;
  }
}
