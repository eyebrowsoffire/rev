#pragma once

namespace rev
{

template <typename ScalarType>
struct Point
{
    ScalarType x;
    ScalarType y;
};

template <typename ScalarType>
struct RectSize
{
    ScalarType width;
    ScalarType height;
};

template <typename ScalarType>
struct Rect
{
    Point<ScalarType> origin;
    RectSize<ScalarType> size;
};

} // namespace rev