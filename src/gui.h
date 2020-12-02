#pragma once
#include <memory>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "renderer.h"

namespace png {
  class GUI {
  private:
    bool show_another_window = false;
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLFWwindow* window_ = nullptr;
    Renderer& renderer;

    void Init(Renderer& pram_renderData);


  public:
    GUI(Renderer& _renderer);
    ~GUI();
    void Update(const std::shared_ptr<RenderTarget> _renderTarget);
    bool IsClose();
  };
}
