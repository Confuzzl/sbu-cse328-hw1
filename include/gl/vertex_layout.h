#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <type_traits>

#include "util.h"

namespace shaders::vertex_layout {
namespace detail {
// template <typename T>
// void attribFormatDispatch(const GLuint vaoID, const GLuint attrIndex,
//                           const GLint size, const GLenum type,
//                           const GLint offset);
template <typename Integer,
          std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
void attribFormatDispatch(const GLuint vaoID, const GLuint attrIndex,
                          const GLint size, const GLenum type,
                          const GLint offset) {
  glVertexArrayAttribIFormat(vaoID, attrIndex, size, type, offset);
}
template <typename Float,
          std::enable_if_t<std::is_same_v<Float, GLfloat>, bool> = true>
void attribFormatDispatch(const GLuint vaoID, const GLuint attrIndex,
                          const GLint size, const GLenum type,
                          const GLint offset) {
  glVertexArrayAttribFormat(vaoID, attrIndex, size, type, false, offset);
}
template <typename Double,
          std::enable_if_t<std::is_same_v<Double, GLdouble>, bool> = true>
void attribFormatDispatch(const GLuint vaoID, const GLuint attrIndex,
                          const GLint size, const GLenum type,
                          const GLint offset) {
  glVertexArrayAttribLFormat(vaoID, attrIndex, size, type, offset);
}

template <typename T>
void enable_helper(const GLuint vaoID, GLuint &attrIndex, GLint &offset) {
  glEnableVertexArrayAttrib(vaoID, attrIndex);
  detail::attribFormatDispatch<typename T::value_type>(
      vaoID, attrIndex, T::length(), GL::macroOf<typename T::value_type>(),
      offset);
  glVertexArrayAttribBinding(vaoID, attrIndex, 0);
  attrIndex++;
  offset += sizeof(T);
}
} // namespace detail

template <typename... VertexTypes> void enable(const GLuint vaoID) {
  GLuint attrIndex = 0;
  GLint offset = 0;
  (detail::enable_helper<VertexTypes>(vaoID, attrIndex, offset), ...);
}

struct alignas(4) pos {
  glm::vec2 _pos;

  /*constexpr*/ pos() = default;
  template <typename X, typename Y>
  /*constexpr*/ pos(const X x, const Y y)
      : _pos{static_cast<float>(x), static_cast<float>(y)} {}
  /*constexpr*/ pos(const glm::vec2 pos) : _pos{pos} {}

  const void *data() const { return &_pos; }
};
template <> void enable<pos>(const GLuint vaoID);

struct alignas(4) postex {
  glm::vec2 pos;
  glm::u16vec2 tex;

  /*constexpr*/ postex() = default;
  template <typename X, typename Y, typename U, typename V>
  /*constexpr*/ postex(const X x, const Y y, const U u, const V v)
      : pos{static_cast<float>(x), static_cast<float>(y)},
        tex{static_cast<unsigned short>(u), static_cast<unsigned short>(v)} {}
  /*constexpr*/ postex(const glm::vec2 pos, const glm::u16vec2 tex)
      : pos{pos}, tex{tex} {}

  const void *data() const { return &pos; }
};

template <> void enable<postex>(const GLuint vaoID);
} // namespace shaders::vertex_layout