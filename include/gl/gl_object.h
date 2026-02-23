#pragma once

#include <glad/gl.h>

#include "util.h"

namespace GL {
struct BufferObject {
  GLuint ID;
  GLsizeiptr size;
  GLsizei count = 0;

  BufferObject(const GLsizeiptr size, const GLbitfield flags,
               const void *data = nullptr);
  ~BufferObject();

  void reset() { count = 0; }
};

struct VBO : BufferObject {
  GLsizei stride;

private:
  VBO(const GLsizei count, const GLsizei stride)
      : BufferObject{static_cast<GLsizeiptr>(count * stride),
                     GL_DYNAMIC_STORAGE_BIT},
        stride{stride} {}

public:
  template <typename T> void write(const T &val) {
    if (sizeof(T) != stride)
      throw std::runtime_error{"WRITING WRONG DATA TYPE/SIZE TO VBO"};
    glNamedBufferSubData(ID, count * stride, stride, &val);
    count++;
  }
  template <typename T> void writeList(const T &list, const bool reset = true) {
    if (sizeof(typename T::value_type) != stride)
      throw std::runtime_error{"WRITING WRONG DATA TYPE/SIZE TO VBO"};

    if (reset)
      count = 0;
    const GLsizei oldCount = count;
    const GLsizei newCount = count + list.size();

    const GLsizei newSize = newCount * stride;
    if (newSize > size) {
      // print_info("RESIZING!");
      fmt::println("COUNTS {} -> {}", oldCount, newCount);
      fmt::println("RESIZING {} -> {}", size, newSize * 2);
      GLuint bigger;
      glCreateBuffers(1, &bigger);
      glNamedBufferStorage(bigger, newSize * 2, nullptr,
                           GL_DYNAMIC_STORAGE_BIT);
      glCopyNamedBufferSubData(ID, bigger, 0, 0, size);
      glDeleteBuffers(1, &ID);
      ID = bigger;
      size = newSize * 2;
    }
    glNamedBufferSubData(ID, oldCount * stride, list.size() * stride,
                         std::data(list));
    count = newCount;
  }

  template <typename T> static VBO create(const GLsizei count) {
    return VBO{count, sizeof(T)};
  }
};
struct EBO : BufferObject {
  GLenum type;

  template <typename T>
  EBO(const T &list)
      : BufferObject{list.size() * sizeof(T::value_type),
                     GL_DYNAMIC_STORAGE_BIT, list.data()},
        type{macroOf<typename T::value_type>()} {}
};
} // namespace GL