#include "renderer.h"

#include <glm/glm.hpp>
#include <imgui.h>

#include "app.h"
#include "color.h"
#include "gl/gl_object.h"
#include "gl/glsl_object.h"
#include "gl/uniform.h"
#include "gl/vertex_layout.h"

Renderer::Renderer()
    : vbo{GL::VBO::create<shaders::vertex_layout::pos>(
          Scene::INITIAL_POINT_CAPACITY)},
      preview{GL::VBO::create<shaders::vertex_layout::pos>(
          Scene::INITIAL_POINT_CAPACITY)} {
  glPointSize(Scene::PIXEL_SCALE);
  glLineWidth(5.0f);
}
void Renderer::render() {
  static auto point = GL::VBO::create<shaders::vertex_layout::pos>(1);

  {
    using namespace shaders::uniforms;
    shaders::getUBO<ViewBlock>().update(ViewBlock{glm::ortho<float>(
        0, app().framebufferSize.x, 0, app().framebufferSize.y, -1, +1)});
  }

  vbo.writeList(app().scene.points);
  shaders.basic.setFragColor(RED).draw(GL_POINTS, vbo);

  if (app().scene.startSet()) {
    preview.writeList(app().scene.generatePreview());
    shaders.basic.setFragColor(YELLOW).draw(GL_POINTS, preview);

    point.write(Scene::asBig(app().scene.smallStart));
    shaders.basic.setFragColor(BLUE).draw(GL_POINTS, point);
  }
  if (!ImGui::GetIO().WantCaptureMouse) {
    point.write(Scene::asBig(app().scene.smallPreview));
    shaders.basic.setFragColor(GREEN).draw(GL_POINTS, point);
  }
}