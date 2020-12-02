
#include "random.h"
#include "ray.h"
#include "texture.h"

#include <cmath>
#include <numbers>
#include <iostream>

namespace png {
  class Aperture {
  public:
    vec3 virtual Sample(Random& _rand) = 0;
  };

  class ApertureDisk : public Aperture {
  public:
    vec3 Sample(Random& _rand) override {
      while (true) {
        png::vec3 p = png::vec3(_rand.next01() * 2.0 - 1.0, _rand.next01() * 2.0 - 1.0, 0);
        if (vec3::Magnitude(p) >= 1) continue;
        return p;
      }
    }
  };

  class ApertureRectangle : public Aperture {
  public:
    vec3 Sample(Random& _rand) override {
      return vec3(2.0f * _rand.next01() - 1.0f, 2.0f * _rand.next01() - 1.0f, 0.0f);
    }
  };

  class AperturePolygonRejection : public Aperture {
  private:
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }

    float sign(vec3 p1, vec3 p2, vec3 p3) {
      return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }

    bool PointInTriangle(vec3 pt, vec3 v1, vec3 v2, vec3 v3) {
      bool b1, b2, b3;

      b1 = sign(pt, v1, v2) < 0.0f;
      b2 = sign(pt, v2, v3) < 0.0f;
      b3 = sign(pt, v3, v1) < 0.0f;

      return ((b1 == b2) && (b2 == b3));
    }

  public:
    vec3 Sample(Random& _rand) override {
      constexpr int polygon = 8;
      std::vector<vec3> coner;
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }

      while (true) {
        double x = _rand.next01() * 2.0 - 1.0;
        double y = _rand.next01() * 2.0 - 1.0;
        for (int i = 0; i < polygon; ++i) {
          vec3 tmp = vec3(x, y, 0);
          if (PointInTriangle(
            tmp,
            vec3(),
            coner[GetIndex(i, polygon)],
            coner[GetIndex(i + 1, polygon)])) {
            return tmp;
          }
        }
      }

      std::cerr << "Out of Area" << std::endl;
      exit(1);
      return coner[0];
    }
  };

  class AperturePolygonBlue : public Aperture {
  private:
    Texture blue;
    int GetIndex(int index, int size) {
      if (index >= size) {
        index -= size;
      }
      return index;
    }
    vec3 Square2triangle(vec3 _input) {
      vec3 cal = _input;
      if (_input.y > _input.x) {
        cal.x *= 0.5f;
        cal.y -= cal.x;
      } else {
        cal.y *= 0.5f;
        cal.x -= cal.y;
      }
      return cal;
    }
  public:
    AperturePolygonBlue()
      : blue(Texture("blue.png")) {
    }
    vec3 Sample(Random& _rand) override {
      constexpr int polygon = 8;
      std::vector<vec3> coner;
      for (int i = 0; i < polygon; ++i) {
        double deg = 2.0 * std::numbers::pi * i / polygon;
        vec3 cal = vec3(std::cos(deg), std::sin(deg), 0);
        coner.push_back(cal);
      }

      double x = _rand.next01();
      double y = _rand.next01();
      const vec3 offset = blue.GetColor(x, y);
      double xo = offset.x;
      double yo = offset.y;

      const unsigned int index = (unsigned int)(x * polygon);


      return vec3();
    }
  };
}
