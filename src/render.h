#pragma once

#include "embree3/rtcore.h"
#include <vector>

namespace png {
  class Renderer {
  private :
    RTCScene sceneHandle;
    RTCDevice deviceHandle;
    RTCIntersectContext context;
  public :
    Renderer();
    ~Renderer();
    void Draw(std::vector<unsigned char>& image, int screen_width, int screen_height);
  };
}
