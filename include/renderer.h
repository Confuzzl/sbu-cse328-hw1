#pragma once

#include "gl/glsl_object.h"
struct Renderer {
  GL::VBO vbo;
  GL::VBO preview;

  struct {
    shaders::Basic basic;
  } shaders;

  Renderer();

  void render();
};