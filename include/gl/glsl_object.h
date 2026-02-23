#pragma once

#define SET_UNIFORM_TEMPLATE(U, T, call)                                       \
  template <>                                                                  \
  inline void setUniform(const Program &p, const GL::Uniform<U> &uniform,      \
                         const T value) {                                      \
    call;                                                                      \
  }
#define SET_SCALAR(type, scalar_type)                                          \
  SET_UNIFORM_TEMPLATE(                                                        \
      type, type,                                                              \
      glProgramUniform1##scalar_type(p.ID, uniform.location, value))
#define SET_VECTOR(type, vector_type)                                          \
  SET_UNIFORM_TEMPLATE(type, type,                                             \
                       glProgramUniform##vector_type##v(                       \
                           p.ID, uniform.location, 1, glm::value_ptr(value)))
#define SET_MATRIX(type, matrix_type)                                          \
  SET_UNIFORM_TEMPLATE(                                                        \
      type, type,                                                              \
      glProgramUniformMatrix##matrix_type##fv(                                 \
          p.ID, uniform.location, 1, GL_FALSE, glm::value_ptr(value)))

#define SET_UNIFORM(func_name, param_t, param_name, shader, def)               \
  auto &set##func_name(const param_t param_name def) {                         \
    setUniform(*this, shader.param_name, param_name);                          \
    return *this;                                                              \
  }
#define SET_UNIFORM_V(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, vertex, def)
#define SET_UNIFORM_F(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, fragment, def)
#define SET_UNIFORM_G(func_name, param_t, param_name, def)                     \
  SET_UNIFORM(func_name, param_t, param_name, geometry, def)

#define BIND_TEXTURE(sampler_name)                                             \
  auto &bindTexture(const GL::Texture &texture) {                              \
    glBindTextureUnit(fragment.sampler_name.binding, texture.ID);              \
    return *this;                                                              \
  }

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "color.h"
#include "gl/component/fragment.h"
#include "gl/component/vertex.h"
#include "gl/gl_object.h"
#include "gl/uniform.h"
#include "gl/vao.h"
#include "util.h"

#include <initializer_list>

namespace GL {
struct Shader {
  GLuint ID;
  GLenum type;
  const char *name;

  Shader(const GLenum type, const char *const name);
  ~Shader();
  MOVE_ONLY_TYPE(Shader)
};

struct Program {
  GLuint ID;

  Program(const std::initializer_list<Shader> shaders);
  ~Program();
  MOVE_ONLY_TYPE(Program)
};
template <typename U, typename T>
void setUniform(const Program &p, const U &uniform, const T value);
SET_SCALAR(bool, i)
SET_SCALAR(int, i)
SET_SCALAR(unsigned int, ui)
SET_SCALAR(Color, ui)
SET_SCALAR(float, f)
SET_VECTOR(glm::vec2, 2f)
SET_VECTOR(glm::uvec2, 2ui)
SET_VECTOR(glm::uvec3, 3ui)
SET_MATRIX(glm::mat4, 4)
} // namespace GL

namespace shaders {
namespace detail {
template <typename V, typename F> struct BaseProgram : ::GL::Program {
protected:
  GL::VertexArrayObject<typename V::LayoutType> vao;
  V vertex;
  F fragment;

  BaseProgram(const std::initializer_list<GL::Shader> shaders)
      : Program{shaders}, vertex{ID}, fragment{ID} {}

private:
  void bind(const GL::VBO &vbo) const {
    glUseProgram(ID);
    glBindVertexArray(vao.ID);
    glVertexArrayVertexBuffer(vao.ID, 0, vbo.ID, 0, vbo.stride);
  }
  void bind(const GL::VBO &vbo, const GL::EBO &ebo) const {
    bind(vbo);
    glVertexArrayElementBuffer(vao.ID, ebo.ID);
  }

public:
  void draw(const GLenum primitive, GL::VBO &vbo,
            const bool reset = true) const {
    bind(vbo);
    glDrawArrays(primitive, 0, vbo.count);
    if (reset)
      vbo.reset();
  }
  void draw(const GLenum primitive, GL::VBO &vbo, const GL::EBO &ebo,
            const bool reset = true) const {
    bind(vbo, ebo);
    glDrawElements(primitive, ebo.count, ebo.type,
                   reinterpret_cast<const void *>(0));
    if (reset)
      vbo.reset();
  }
};
template <typename V, typename F> struct SimpleProgram : BaseProgram<V, F> {
  SimpleProgram()
      : BaseProgram<V, F>(
            {{GL_VERTEX_SHADER, V::name}, {GL_FRAGMENT_SHADER, F::name}}) {}
};
} // namespace detail

struct Basic : detail::SimpleProgram<vert::Basic, frag::Basic> {
  SET_UNIFORM_F(FragColor, Color, frag_color, )
};
} // namespace shaders

#undef SET_UNIFORM_TEMPLATE
#undef SET_SCALAR
#undef SET_VECTOR_2
#undef SET_VECTOR
#undef SET_MATRIX
#undef SET_UNIFORM
#undef SET_UNIFORM_V
#undef SET_UNIFORM_F
#undef SET_UNIFORM_G
#undef BIND_TEXTURE