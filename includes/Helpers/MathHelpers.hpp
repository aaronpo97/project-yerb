#pragma once
#include <cmath>
namespace MathHelpers {
  const float pi = atan(1) * 4;
  float       degreesToRadians(const float degrees);
  float       radiansToDegrees(const float radians);
  float       pythagoras(const float a, const float b);
}; // namespace MathHelpers