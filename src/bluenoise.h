
#include "texture.h"

namespace png {
  class BlueNoiseSampler {
  private:
    Texture blueNoise;
  public:
    BlueNoiseSampler(Texture _tex);
    vec3 Sample(double x, double y);
  };
}
