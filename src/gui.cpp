#include "gui.h"
#include "RenderTarget.h"
#include "g_data.h"
#include <iostream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//=============================================================================
//=============================================================================
//=============================================================================
static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

//=============================================================================
//=============================================================================
//=============================================================================
png::GUI::GUI() {
  Init();
}

//=============================================================================
//=============================================================================
//=============================================================================
png::GUI::~GUI() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

//=============================================================================
//=============================================================================
//=============================================================================
static bool ImGuiComboUI(const std::string& caption, std::string& current_item,
                         const std::vector<std::string>& items) {
  bool changed = false;

  if (ImGui::BeginCombo(caption.c_str(), current_item.c_str())) {
    for (int n = 0; n < items.size(); n++) {
      bool is_selected = (current_item == items[n]);
      if (ImGui::Selectable(items[n].c_str(), is_selected)) {
        current_item = items[n];
        changed = true;
      }
      if (is_selected) {
        // Set the initial focus when opening the combo (scrolling + for
        // keyboard navigation support in the upcoming navigation branch)
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  return changed;
}

//=============================================================================
//=============================================================================
//=============================================================================
void png::GUI::Init() {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    exit(1);
  }

  // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
  window_ = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
  if (window_ == NULL) {
    exit(1);

  }
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1); // Enable vsync

  // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
  bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
  bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
  bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
  bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
  bool err = false;
  glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
  bool err = false;
  glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
  bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    exit(1);
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);
}

//=============================================================================
//=============================================================================
//=============================================================================
void png::GUI::Update() {
  // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    //Singleton<png::G_Data>::singleton().cam.target = f;
  }

  {
    ImGui::Begin("Debug");

    //stop
    {
      ImGuiIO& io = ImGui::GetIO();

      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
        if (ImGui::IsKeyReleased(i) && i == 32) {
          //ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i);
          Singleton<png::G_Data>::singleton().IsRender = !Singleton<png::G_Data>::singleton().IsRender;
        }
      }
    }

    //info
    {
      ImGui::Text("sample :%d", Singleton<png::G_Data>::singleton().renderTex->sampleCounter);
    }
    // texutre
    {
      //ImGui::SliderFloat("Texture Size", &textureSize, 0.01f, 10.0f);
      static int width = Singleton<png::G_Data>::singleton().renderTex->width;
      if (ImGui::InputInt("Width", &width, 10, 100)) {
        Singleton<png::G_Data>::singleton().renderTex->width = width;
        Singleton<png::G_Data>::singleton().renderTex->height = width;
        Singleton<png::G_Data>::singleton().Change();
      }
      static int tmp_superSampling = Singleton<png::G_Data>::singleton().renderTex->superSampling;
      if (ImGui::InputInt("Super Sampling", &tmp_superSampling, 1, 10)) {
        Singleton<png::G_Data>::singleton().renderTex->superSampling = tmp_superSampling;
        Singleton<png::G_Data>::singleton().Change();
      }
    }
    // camera type
    {
      const char* items[] = { "Path Tracing","Primal Ray Tracing","Lambert Diffuse" };
      static int item_current = 0;
      if (ImGui::Combo("combo", &item_current, items, sizeof(items) / sizeof(char*))) {
        Singleton<png::G_Data>::singleton().cam.type = item_current;
        Singleton<png::G_Data>::singleton().Change();
      }
    }
    // hoge 
    {
      std::vector<std::string> items{ "a","b","c" };
      static int item_current = 0;
      static std::string caption;
      static std::string current_item;
      //if (ImGuiComboUI());
    }
    //camera origin
    {
      // Axis-aligned param
      png::vec3& org = Singleton<png::G_Data>::singleton().cam.origin;
      float vec3[3] = { org.x, org.y, org.z };
      if (ImGui::InputFloat3("Camera Origin", vec3)) {
        Singleton<png::G_Data>::singleton().cam.origin = png::vec3(vec3[0], vec3[1], vec3[2]);
        Singleton<png::G_Data>::singleton().Change();
      }

      // degree param
      const float length = std::sqrtf(org.x * org.x + org.y * org.y + org.z * org.z);
      float tmp_cal = 0.0f;
      {
        int tmp_sign = ((0.0f < org.x) - (org.x < 0.0f)) + 1;
        float tmp_theta = std::acosf(org.z / std::sqrtf(org.z * org.z + org.x * org.x)) / std::numbers::pi;
        if (tmp_sign) {
          tmp_cal = tmp_theta * 0.5f;
        } else {
          tmp_cal = 1.0f - tmp_theta * 0.5f;
        }
      }
      float theta[3] = {
        acosf(org.y / length) / std::numbers::pi,
        tmp_cal ,
        length
      };
      if (ImGui::SliderFloat("theta", &theta[0], 0.0f, 1.0f) || ImGui::SliderFloat("phi", &theta[1], 0.0f, 1.0f) || ImGui::SliderFloat("length", &theta[2], 0, 10)) {
        float thetaPI = theta[0] * std::numbers::pi;
        float phiPI = theta[1] * 2.0f * std::numbers::pi;
        if (-0.001f <= thetaPI && thetaPI <= 0.001f) {
          thetaPI = 0.001f;
        }
        if (-0.001f <= thetaPI && phiPI <= 0.001f) {
          phiPI = 0.001f;
        }
        png::vec3 hoge = png::vec3(
          std::sinf(thetaPI) * std::sinf(phiPI),
          std::cosf(thetaPI),
          std::sinf(thetaPI) * std::cosf(phiPI)
        ) * theta[2];
        Singleton<png::G_Data>::singleton().cam.origin = hoge;
        Singleton<png::G_Data>::singleton().Change();
      }
    }

    //camera target
    {
      png::vec3 target = Singleton<png::G_Data>::singleton().cam.target;
      float vec3[3] = { target.x, target.y, target.z };
      if (ImGui::InputFloat3("Camera Target", vec3)) {
        Singleton<png::G_Data>::singleton().cam.target = png::vec3(vec3[0], vec3[1], vec3[2]);
        Singleton<png::G_Data>::singleton().Change();
      }
    }

    //camera fov
    {
      if (ImGui::SliderFloat("Camera FOV", &Singleton<png::G_Data>::singleton().cam.fov, 0.0f, 10.0f)) {
        Singleton<png::G_Data>::singleton().Change();
      }
    }

    ImGui::End();
  }

  {
    png::RenderTarget* renderTarget = Singleton<png::G_Data>::singleton().renderTex;
    ImGui::Begin("OpenGL Texture Text");
    ImGui::Text("size = %d x %d", renderTarget->width, renderTarget->height);
    ImGui::Image((void*)(intptr_t)(*(renderTarget->image_id)), ImVec2(500, 500));
    ImGui::End();
  }

  // 3. Show another simple window.
  if (show_another_window) {
    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
      show_another_window = false;
    ImGui::End();
  }

  // Rendering
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(window_, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window_);
}

//=============================================================================
//=============================================================================
//=============================================================================
bool png::GUI::IsClose() {
  return glfwWindowShouldClose(window_);
}
