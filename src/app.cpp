#include "app.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <imgui.h>

#include <stdexcept>

#include "callback.h"
#include "scene.h"
#include "util.h"

Initializer::Initializer() {
  if (!glfwInit()) {
    throw std::runtime_error{"GLFW FAILED TO INIT"};
  }

  glfwSetErrorCallback(callback::error);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  window = glfwCreateWindow(DEFAULT_DIMENSIONS.x, DEFAULT_DIMENSIONS.y,
                            "Template", nullptr, nullptr);
  if (!window) {
    throw std::runtime_error{"GLFW FAILED TO CREATE WINDOW"};
  }

  glfwGetFramebufferSize(window, &framebufferSize.x, &framebufferSize.y);
  glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(static_cast<GLADloadfunc>(glfwGetProcAddress))) {
    throw std::runtime_error{"GLAD FAILED TO LOAD"};
  }

  glfwSetFramebufferSizeCallback(window, callback::framebufferSize);
  glfwSetWindowSizeCallback(window, callback::windowSize);

  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    app().input.keyCallback(window, key, scancode, action, mods);
  });
  glfwSetCursorPosCallback(window,
                           [](GLFWwindow *window, double xpos, double ypos) {
                             app().input.cursorPosCallback(window, xpos, ypos);
                           });
  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *window, int button, int action, int mods) {
        app().input.mouseButtonCallback(window, button, action, mods);
      });
  glfwSetScrollCallback(window,
                        [](GLFWwindow *window, double xpos, double ypos) {
                          app().input.scrollCallback(window, xpos, ypos);
                        });

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(callback::debug, 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450");
  ImGui::StyleColorsDark();
}
Initializer::~Initializer() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // glfwDestroyWindow(window);
  // glfwTerminate();
}

void App::run() {
  glfwMaximizeWindow(window);

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGuiIO &io = ImGui::GetIO();

  double prevT = glfwGetTime(), currT = prevT;
  while (!glfwWindowShouldClose(window)) {
    currT = glfwGetTime();
    const double dt = currT - prevT;

    glfwPollEvents();
    input.processKeys(dt);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::SetNextWindowPos({0, 0});
      ImGui::SetNextWindowSize({200, static_cast<float>(app().windowSize.y)});
      ImGui::SetNextWindowBgAlpha(0.5f);
      ImGui::Begin("hello", nullptr,
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_NoTitleBar);

      // ImGui::ShowDemoWindow();
      auto foo = fmt::format("dt = {:.4f}", dt);
      ImGui::Text(foo.c_str());

      // foo = fmt::format("vbo count = {}", renderer.vbo.count);
      // ImGui::Text(foo.c_str());
      // foo = fmt::format("pvbo count = {}", renderer.preview.count);
      // ImGui::Text(foo.c_str());

      // foo = fmt::format("want capture mouse = {}",
      //                   ImGui::GetIO().WantCaptureMouse);
      // ImGui::Text(foo.c_str());

      foo =
          fmt::format("start = {} {}", scene.smallStart.x, scene.smallStart.y);
      ImGui::Text(foo.c_str());
      foo = fmt::format("preview = {} {}", scene.smallPreview.x,
                        scene.smallPreview.y);
      ImGui::Text(foo.c_str());
      // foo = fmt::format("start = {} {}", scene.start.x, scene.start.y);
      // ImGui::Text(foo.c_str());
      // foo = fmt::format("preview = {} {}", scene.preview.x, scene.preview.y);
      // ImGui::Text(foo.c_str());

      foo = fmt::format("commited = {}", scene.points.size());
      ImGui::Text(foo.c_str());

      foo = fmt::format("connect poly = {}", scene.connectPoly);
      ImGui::Text(foo.c_str());

      foo = fmt::format("circle = {}", scene.circle);
      ImGui::Text(foo.c_str());

      struct ButtonData {
        const char *name;
      };
      static std::map<Scene::ToolState, ButtonData> buttonData{
          {Scene::ToolState::LINE, {"1: Line"}},
          {Scene::ToolState::POLYLINE, {"3: Polyline"}},
          {Scene::ToolState::ELLIPSE, {"4: Ellipse"}},
          {Scene::ToolState::BONUS, {"BONUS"}},
      };

      for (const auto [state, data] : buttonData) {
        static constexpr auto button = [](auto name) {
          static constexpr ImVec2 SIZE{100, 100};
          ImGui::SetCursorPosX((ImGui::GetWindowSize().x - SIZE.x) / 2);
          return ImGui::Button(name, SIZE);
        };
        if (scene.toolstate == state) {
          ImGui::BeginDisabled();
          button(data.name);
          ImGui::EndDisabled();
        } else {
          if (button(data.name)) {
            scene.setState(state);
          }
        }
      }

      if (ImGui::Button("CLEAR", {ImGui::GetContentRegionAvail().x, 50})) {
        scene.points.clear();
      }

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    renderer.render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    prevT = currT;
  }
}
void App::close() { glfwSetWindowShouldClose(window, true); }

App &app() {
  static App instance{};
  return instance;
}