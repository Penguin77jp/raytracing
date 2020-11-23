#pragma once
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "renderData.h"

namespace png {
  class GUI {
  private :
    bool show_another_window = false;
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLFWwindow* window_ = nullptr;
    RenderData& renderData;

    void Init(RenderData& pram_renderData);


  public :
    GUI(RenderData& pram_renderData);
    ~GUI();
    void Update();
    bool IsClose();
};
}
