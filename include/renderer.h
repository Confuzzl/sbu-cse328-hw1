#pragma once

#include "gl/glsl_object.h"
struct Renderer {
  GL::VBO commited;
  GL::VBO preview;
  GL::VBO bonus;

  bool showAxes = true;

  struct {
    shaders::Basic basic;
  } shaders;

  Renderer();

  void render();
};