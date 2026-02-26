#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "input/input_handler.h"
#include "renderer.h"
#include "scene.h"

struct Initializer {
  GLFWwindow *window = nullptr;
  static constexpr glm::ivec2 DEFAULT_DIMENSIONS{1280, 720};
  float windowScale;
  glm::ivec2 framebufferSize;
  glm::ivec2 windowSize;

  Initializer();
  ~Initializer();
};
struct App : Initializer {
  Renderer renderer;
  InputHandler input;

  Scene scene;

  void run();
  void close();
};

App &app();
