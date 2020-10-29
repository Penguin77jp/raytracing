#pragma once
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace png {
  class GUI {
  private :
    void Init();

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLFWwindow* window_ = nullptr;

  public :
    GUI();
    ~GUI();
    void Update();
    bool IsClose();
};
}
