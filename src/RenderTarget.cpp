#include "renderTarget.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
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
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <cmath>
#include <iostream>
#include <filesystem>

png::RenderTarget::RenderTarget(const uint16_t w, const uint16_t h)
  : width(w), height(h),
  image_id(nullptr),
  sampleCounter(0),
  image_dataF(4 * width * height, 0.0f),
  image_data(image_dataF.size(), 0) {

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Upload pixels into texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());

  //*image_id = (void*)image_texture;
  image_id = new GLuint(image_texture);
}

png::RenderTarget::RenderTarget(const RenderTarget& _renderTarget)
  : width(_renderTarget.width)
  , height(_renderTarget.height)
  , image_id(nullptr)
  , sampleCounter(0)
  , image_dataF(4 * width * height, 0.0f)
  , image_data(image_dataF.size(), 0) {
}

void png::RenderTarget::Update() {
  glDeleteTextures(1, image_id);
  delete image_id;

  sampleCounter++;
#ifdef _RELEASE
#pragma omp parallel for
#endif
  for (int i = 0; i < image_data.size(); ++i) {
    float val = image_dataF[i] / sampleCounter;
    if (val > 1.0f) {
      val = 1.0f;
    }
    //image_data[i] = (unsigned char)(255 * val);
    image_data[i] = (unsigned char)(255 * std::pow(val, 1.0f / 2.2f));
  }

  // Create a OpenGL texture identifier
  GLuint image_texture;
  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Upload pixels into texture
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data.data());
  image_id = new GLuint(image_texture);
}

void png::RenderTarget::Init() {
  image_data.resize(4 * width * height);
  image_dataF.resize(image_data.size());
  for (int i = 0; i < image_data.size(); ++i) {
    image_data[i] = image_dataF[i] = 0;
  }
  sampleCounter = 0;
}

void png::RenderTarget::WriteImage(std::string fileName) {
  std::vector<unsigned char> tmp(4 * width * height, 0);
  for (int i = 0; i < width * height * 4; ++i) {
    float val = std::powf(image_dataF[i] / sampleCounter, 1.0 / 2.2f);
    if (val > 1) {
      val = 1.0f;
    }
    tmp[i] = (unsigned char)(255 * val);
  }
  //check directory
  using namespace std::filesystem;
  std::string imageDirString("./Image");
  directory_entry imageDir(imageDirString);
  if (!imageDir.exists() || !imageDir.is_directory()) {
    create_directory(imageDir.path());
  }
  std::string path = imageDirString + std::string("/") + fileName + std::string(".png");
  stbi_write_png(path.c_str(), width, height, 4, &tmp[0], 0);
  std::cout << "saved " << path << std::endl;
}
