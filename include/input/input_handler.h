#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <map>

#include "input/key.h"

struct InputHandler {
  glm::ivec2 cursorPos{0, 0};
  std::map<int, Key> keys;

  InputHandler();

  glm::ivec2 snappedCursorPos() const;

  void processKeys(const double dt);

  void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
  void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
  void mouseButtonCallback(GLFWwindow *window, int button, int action,
                           int mods);
  void scrollCallback(GLFWwindow *window, double xpos, double ypos);
};