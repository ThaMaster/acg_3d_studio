#ifndef _WIN32
/*
Workaround for some kind of linker bug. See https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
*/
#include <pthread.h>
void junk() {
  int i;
  i = pthread_getconcurrency();
};
#endif


#ifdef _WIN32
#include <windows.h>
#endif

// Important: Import glad as first gl header
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include <vr/Application.h>
#include <vr/Version.h>
#include <glm/vec2.hpp>
#include <vr/glErrorUtil.h>

// Weak pointer to the application, so we can access it in callbacks
std::weak_ptr<vr::Application> g_applicationPtr;

void scroll_mouse_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if (auto app = g_applicationPtr.lock())
  {
    float fov = app->getCamera()->getFOV();
    fov += (float)yoffset;
    app->getCamera()->setFOV(fov);

  }
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  int shouldClose = 0;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  if (key == GLFW_KEY_R && action == GLFW_PRESS)
    if (auto app = g_applicationPtr.lock())
      app->reloadScene();

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
  if (auto app = g_applicationPtr.lock())
    app->setScreenSize(width, height);
}


GLFWwindow* initializeWindows(int width, int height)
{
  GLFWwindow* window = nullptr;

  // Initialize GLFW
  if (!glfwInit())
    return nullptr;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "ObjViewer", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, keyboard_callback);

  glfwSetScrollCallback(window, scroll_mouse_callback);

  glfwSetWindowSizeCallback(window, window_size_callback);


  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    std::cerr << "Error: " << gl_error_string(glGetError()) << std::endl;
    glfwTerminate();
    glfwDestroyWindow(window);
    return nullptr;
  }

  printf("GL version: %s\n", glGetString(GL_VERSION));
  printf("GL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  return window;
}

void cleanupWindows(GLFWwindow *window)
{
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
}


int main(int argc, char** argv) 
{
  const unsigned SCREEN_WIDTH = 1920;
  const unsigned SCREEN_HEIGHT = 1080;

  std::cerr << vr::getName() << ": " << vr::getVersion() << std::endl << std::endl;

  GLFWwindow *window = initializeWindows(SCREEN_WIDTH, SCREEN_HEIGHT);

  std::shared_ptr<vr::Application> application = std::make_shared<vr::Application>(SCREEN_WIDTH, SCREEN_HEIGHT);
  g_applicationPtr = application;

  std::string model_filename = (char*) "models/porsche_911_gt3.glb";
  if (argc > 1)
    model_filename = argv[1];

  std::string v_shader_filename = "shaders/phong-shading.vs";
  std::string  f_shader_filename = "shaders/phong-shading.fs";

  if (argc < 2 ) {
    std::cerr << "Loading default model: " << model_filename << std::endl;
    std::cerr << "\n\nUsage: " << argv[0] << " <model-file>" << std::endl;
  }

  if (!application->initResources(model_filename, v_shader_filename, f_shader_filename))
  {
    cleanupWindows(window);
    return 1;
  }

  application->setClearColor(glm::f32vec4(0.1, 0.1, 0.3, 1.0));
  application->getCamera()->setFOV(60);
  application->initView();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window))
  {
    application->update(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
    application->processInput(window);
  }
  
  // Shutdown the application BEFORE we loose OpenGL Context
  application = nullptr;

  cleanupWindows(window);

  return 0;
}

