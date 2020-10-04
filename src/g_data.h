#pragma once
#include "Singleton.h"
#include "texture.h"
#include "camera.h"
#include <iostream> 

namespace png {
  class G_Data : public Singleton<G_Data> {
  public:
    png::Texture* renderTex;
    png::Camera cam;
    void Change() {
      cam.Change();
    }

  private:
    friend class Singleton<G_Data>;
    G_Data() : renderTex(nullptr), cam(png::Camera{}) {}
  };
}
