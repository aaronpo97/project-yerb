#include "../includes/Vec2.h"
#include <cmath>

Vec2::Vec2(float x, float y) :
    x(x), y(y) {}

bool Vec2::operator==(const Vec2 &rhs) const {
  return rhs.x == x && rhs.y == y;
}

bool Vec2::operator!=(const Vec2 &rhs) const {
  return !(*this == rhs);
}

Vec2 Vec2::operator-(const Vec2 &rhs) const {
  return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator+(const Vec2 &rhs) const {
  return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator*(const float val) const {
  return Vec2(x * val, y * val);
}

Vec2 Vec2::operator/(const float val) const {
  return Vec2(x / val, y / val);
}

void Vec2::operator+=(const Vec2 &rhs) {
  x += rhs.x;
  y += rhs.y;
}

void Vec2::operator-=(const Vec2 &rhs) {
  x -= rhs.x;
  y -= rhs.y;
}

void Vec2::operator*=(const float val) {
  x *= val;
  y *= val;
}

void Vec2::operator/=(const float val) {
  x /= val;
  y /= val;
}

float Vec2::length() const {
  return std::sqrt(x * x + y * y);
}

// the normalize function will make the vector a unit vector (length of 1) pointing in the
// same direction
Vec2 Vec2::normalize() {
  float len = length();
  if (len > 0) { // Avoid division by zero
    x /= len;
    y /= len;
  }

  return *this;
}