#pragma once

#include <glm/glm.hpp>

#include <vector>

struct Scene {
  static constexpr auto PIXEL_SCALE = 4;

  Scene();

  enum struct ToolState {
    LINE,
    POLYLINE,
    ELLIPSE,
  } toolstate = ToolState::LINE;

  // private:
  static constexpr auto INITIAL_POINT_CAPACITY = 4096;
  std::vector<glm::vec2> points{};
  std::vector<glm::vec2> previewPoints{};

  glm::ivec2 smallStart = {-1, -1};
  glm::ivec2 smallPreview{};

  static glm::vec2 asBig(const glm::ivec2 p) {
    return {p.x * PIXEL_SCALE, p.y * PIXEL_SCALE};
  }

  std::vector<glm::ivec2> polyPoints{};
  bool connectPoly = false;
  void addPolyPoint(const glm::ivec2 p);

  bool circle = false;


  glm::vec3 quadraticParams{1};
  glm::vec4 cubicParams{1};
  glm::vec3 superParams{1};

  bool startSet() const { return smallStart != glm::ivec2{-1, -1}; }
  void resetStart() { smallStart = {-1, -1}; }

  const std::vector<glm::vec2> &generatePreview();

  void commitEndPoints();

  void setState(const ToolState next);
  // void addPoint(const glm::vec2 point);
};
