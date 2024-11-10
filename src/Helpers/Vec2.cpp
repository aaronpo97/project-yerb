#include "../../includes/Helpers/Vec2.hpp"
#include <cmath>
#include <iostream>

Vec2::Vec2(const float x, const float y) :
    x(x), y(y) {}

bool Vec2::operator==(const Vec2 &rhs) const {
  return rhs.x == x && rhs.y == y;
}

bool Vec2::operator!=(const Vec2 &rhs) const {
  return !(*this == rhs);
}

Vec2 Vec2::operator-(const Vec2 &rhs) const {
  return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator+(const Vec2 &rhs) const {
  return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator*(const float val) const {
  return {x * val, y * val};
}

Vec2 Vec2::operator/(const float val) const {
  return {x / val, y / val};
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
  const float len = length();
  if (len > 0) { // Avoid division by zero
    x /= len;
    y /= len;
  }

  return *this;
}

std::ostream &operator<<(std::ostream &os, const Vec2 &vec2) {
  os << "Vec2(" << vec2.x << ", " << vec2.y << ")";
  return os;
}