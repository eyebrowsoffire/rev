#pragma once

#include <chrono>

namespace rev {

using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::steady_clock::duration;

template <typename ScalarType> struct Point {
  ScalarType x;
  ScalarType y;
};

template <typename ScalarType> struct RectSize {
  ScalarType width;
  ScalarType height;
};

template <typename ScalarType> struct Rect {
  Point<ScalarType> origin;
  RectSize<ScalarType> size;
};

} // namespace rev