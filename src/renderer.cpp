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
    : commited{GL::VBO::create<shaders::vertex_layout::pos>(
          Scene::INITIAL_POINT_CAPACITY)},
      preview{GL::VBO::create<shaders::vertex_layout::pos>(
          Scene::INITIAL_POINT_CAPACITY)},
      bonus{GL::VBO::create<shaders::vertex_layout::pos>(
          Scene::INITIAL_POINT_CAPACITY)} {
  glPointSize(Scene::PIXEL_SCALE);
  glLineWidth(Scene::PIXEL_SCALE);
}
void Renderer::render() {
  static auto point = GL::VBO::create<shaders::vertex_layout::pos>(1);
  static auto axes = GL::VBO::create<shaders::vertex_layout::pos>(2);

  {
    using namespace shaders::uniforms;
    shaders::getUBO<ViewBlock>().update(ViewBlock{glm::ortho<float>(
        0, app().framebufferSize.x, 0, app().framebufferSize.y, -1, +1)});
  }

  if (showAxes && app().scene.bonusState != Scene::BonusState::NONE) {
    const auto offset = Scene::asBig(app().scene.bonusOffset);
    axes.write(glm::vec2{0, offset.y});
    axes.write(glm::vec2{app().framebufferSize.x, offset.y});
    shaders.basic.setFragColor(BLACK).draw(GL_LINES, axes);
    axes.write(glm::vec2{offset.x, 0});
    axes.write(glm::vec2{offset.x, app().framebufferSize.y});
    shaders.basic.setFragColor(BLACK).draw(GL_LINES, axes);
  }

  bonus.writeList(app().scene.generateBonus());
  shaders.basic.setFragColor(ORANGE).draw(GL_POINTS, bonus);

  commited.writeList(app().scene.commitedPoints);
  shaders.basic.setFragColor(RED).draw(GL_POINTS, commited);

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