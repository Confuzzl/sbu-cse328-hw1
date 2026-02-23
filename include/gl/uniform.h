#pragma once

#include <fmt/base.h>
#include <fmt/format.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdexcept>

#include "gl/gl_object.h"
#include "util.h"

namespace shaders {
struct UniformHolder { // TO BE DERIVED BY INDIVIDUAL SHADER TYPES
  GLuint programID;

  UniformHolder(const GLuint programID);
};
} // namespace shaders

namespace GL {
template <typename T> struct UBO : BufferObject {
  T data;

  UBO() : BufferObject(sizeof(T), GL_DYNAMIC_STORAGE_BIT) {}
  template <typename U> void update(U &&x) {
    data = std::forward<U>(x);
    glNamedBufferSubData(ID, 0, sizeof(T), &data);
  }
};

template <typename T> struct Uniform {
  GLuint programID;
  GLint location;

  Uniform(const GLuint programID, const char *name)
      : programID{programID}, location{glGetUniformLocation(programID, name)} {
    if (location == -1)
      throw std::runtime_error{
          fmt::format("{} | {} was not a valid uniform name [{}]", programID,
                      name, GL::getErrorName())};
    fmt::println("{} | {}:{}", programID, name, location);
  }

#define UNIFORM_GET_SCALAR(gl_type, type, scalar_type)                         \
  gl_type get##type() const {                                                  \
    gl_type out;                                                               \
    glGetUniform##scalar_type##v(programID, location, &out);                   \
    return out;                                                                \
  }
#define UNIFORM_GET_VECTOR(gl_type, type, vector_type)                         \
  template <std::size_t L> auto getVector##type() const {                      \
    glm::vec<L, gl_type> out;                                                  \
    glGetnUniform##scalar_type##v(programID, location, sizeof(out),            \
                                  glm::value_ptr(out));                        \
    return out;                                                                \
  }

  UNIFORM_GET_SCALAR(GLint, Int, i)
  UNIFORM_GET_SCALAR(GLuint, Uint, ui)
  UNIFORM_GET_SCALAR(GLfloat, Float, f)
  UNIFORM_GET_SCALAR(GLdouble, Double, d)

  UNIFORM_GET_VECTOR(GLint, Int, i)
  UNIFORM_GET_VECTOR(GLuint, Uint, ui)
  UNIFORM_GET_VECTOR(GLfloat, Float, f)
  UNIFORM_GET_VECTOR(GLdouble, Double, d)

#undef UNIFORM_GET_SCALAR
#undef UNIFORM_GET_VECTOR
}; // namespace GL

struct Sampler {
  GLuint binding;

  Sampler(const GLuint programID, const char *name);

private:
  static GLuint getBinding(const GLuint shaderID, const char *name);
};
} // namespace GL

namespace shaders {
namespace uniforms {
struct ViewBlock {
  static constexpr char name[] = "ViewBlock";
  glm::mat4 view;
};
} // namespace uniforms

namespace detail {
extern GLuint BINDINGS;
} // namespace detail
template <typename UniformType> GLuint getBinding() {
  static auto out = detail::BINDINGS++;
  return out;
}

template <typename UniformType> GL::UBO<UniformType> &getUBO() {
  static GL::UBO<UniformType> out{};
  static auto binding = getBinding<UniformType>();
  return out;
}
} // namespace shaders

namespace GL {
template <typename UniformType> struct UniformBlock {
  GLuint blockIndex;

  UniformBlock(const GLuint programID)
      : blockIndex{glGetUniformBlockIndex(programID, UniformType::name)} {
    if (blockIndex == GL_INVALID_INDEX)
      throw std::runtime_error{
          fmt::format("invalid block index for {}", UniformType::name)};

    const GLuint binding = shaders::getBinding<UniformType>();
    print_info(fmt::format("{} | {}:block={}, binding={}", programID,
                           UniformType::name, blockIndex, binding));
    glUniformBlockBinding(programID, blockIndex, binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding,
                     shaders::getUBO<UniformType>().ID);
  }
};
} // namespace GL