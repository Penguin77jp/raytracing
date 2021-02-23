#pragma once

#include <string>
#include <cmath>

namespace png {
  template <typename T = double>
  class vec3 {
  public:
    double x, y, z;
    vec3() :x(0), y(0), z(0) {}
    vec3(T x, T y, T z) :x(x), y(y), z(z) {}
    vec3(const vec3& a) : x(a.x), y(a.y), z(a.z) {}

    //operator
    vec3 operator +(vec3 a) const {
      return vec3(this->x + a.x, this->y + a.y, this->z + a.z);
    }
    vec3 operator -(const vec3& a) const {
      return vec3(this->x - a.x, this->y - a.y, this->z - a.z);
    }
    vec3 operator -() const {
      return vec3(-this->x, -this->y, -this->z);
    }
    vec3 operator *(const vec3& a) const {
      return vec3(this->x * a.x, this->y * a.y, this->z * a.z);
    }
    vec3 operator* (T a) const {
      return vec3(this->x * a, this->y * a, this->z * a);
    }
    vec3 operator /(const vec3& a) const {
      return vec3(this->x / a.x, this->y / a.y, this->z / a.z);
    }
    vec3 operator /(const T& b) const {
      return vec3(this->x / b, this->y / b, this->z / b);
    }
    vec3& operator +=(const vec3& b) {
      this->x += b.x;
      this->y += b.y;
      this->z += b.z;
      return *this;
    }
    vec3& operator -=(const vec3& b) {
      this->x -= b.x;
      this->y -= b.y;
      this->z -= b.z;
      return *this;
    }
    operator std::string() const {
      return std::to_string(this->x) + std::string(" , ") + std::to_string(this->y) + std::string(" , ") + std::to_string(this->z);
    }

    static T Dot(vec3 a, vec3 b) {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    static vec3 Cross(vec3 a, vec3 b) {
      return vec3(a.y * b.z - a.z * b.y, -a.x * b.z + a.z * b.x, a.x * b.y - a.y * b.x);
    }
    static T Magnitude(vec3 a) {
      return std::sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    }
    static vec3 Normalize(vec3 a) {
      return  a / Magnitude(a);
    }
  };

  template <typename T = double>
  class Ray {
  public:
    vec3<T> org, dir;
    Ray(vec3<T> o, vec3<T> d) :org(o), dir(d) {}
    Ray() :org(vec3<T>()), dir(vec3<T>()) {}
  };
}
