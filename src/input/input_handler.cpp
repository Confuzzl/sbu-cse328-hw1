#include "input/input_handler.h"

#include <imgui.h>

#include <algorithm>

#include "app.h"

InputHandler::InputHandler()
    : keys{
          {GLFW_KEY_ESCAPE, Key{[](const double) { app().close(); }}},
          {GLFW_KEY_1, Key{[](const double) {
             app().scene.setState(Scene::ToolState::LINE);
           }}},
          {GLFW_KEY_3, Key{[](const double) {
             app().scene.setState(Scene::ToolState::POLYLINE);
           }}},
          {GLFW_KEY_4, Key{[](const double) {
             app().scene.setState(Scene::ToolState::ELLIPSE);
           }}},
          {GLFW_KEY_C,
           Key{[](const double) { app().scene.connectPoly = true; },
               [](const double) { app().scene.connectPoly = false; }}},
          {GLFW_KEY_LEFT_SHIFT,
           Key{[](const double) { app().scene.circle = true; },
               [](const double) { app().scene.circle = false; }}},
      } {}
glm::ivec2 InputHandler::snappedCursorPos() const {
  return {cursorPos.x / Scene::PIXEL_SCALE/* * Scene::PIXEL_SCALE +
              Scene::PIXEL_SCALE / 2*/,
          (app().windowSize.y - cursorPos.y) / Scene::PIXEL_SCALE /**
                  Scene::PIXEL_SCALE +
              Scene::PIXEL_SCALE / 2*/};
}

void InputHandler::processKeys(const double dt) {
  for (auto &[keycode, key] : keys)
    key(dt);
}
void InputHandler::keyCallback(GLFWwindow *window, int key, int scancode,
                               int action, int mods) {
  if (const auto iterator = keys.find(key); iterator != keys.cend())
    iterator->second.react(action);
}
void InputHandler::cursorPosCallback(GLFWwindow *window, double xpos,
                                     double ypos) {
  cursorPos = {std::clamp<double>(xpos, 0, app().windowSize.x),
               std::clamp<double>(ypos, 0, app().windowSize.y)};
  app().scene.smallPreview = snappedCursorPos();
}
void InputHandler::mouseButtonCallback(GLFWwindow *window, int button,
                                       int action, int mods) {
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  if (action == GLFW_PRESS) {
    const auto snapped = snappedCursorPos();
    switch (app().scene.toolstate) {
    case Scene::ToolState::LINE: {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app().scene.smallStart = snapped;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        app().scene.commitEndPoints();
      }
      break;
    }
    case Scene::ToolState::POLYLINE: {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app().scene.addPolyPoint(snapped);
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        app().scene.commitEndPoints();
      }
      break;
    }
    case Scene::ToolState::ELLIPSE: {
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app().scene.smallStart = snapped;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        app().scene.commitEndPoints();
      }
      break;
    }
    case Scene::ToolState::BONUS: {
      break;
    }
    }
  }
}
void InputHandler::scrollCallback(GLFWwindow *window, double xpos,
                                  double ypos) {}