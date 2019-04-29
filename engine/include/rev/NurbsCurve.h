#pragma once

#include "rev/Utilities.h"
#include <algorithm>
#include <cstddef>
#include <glm/glm.hpp>
#include <gsl/gsl_assert>
#include <gsl/span>
#include <limits>

namespace rev {

template <typename PointType> struct WeightedControlPoint {
  PointType point;
  typename PointType::value_type weight;
};

template <typename PointType> class NurbsCurve {
public:
  using ControlPointType = WeightedControlPoint<PointType>;
  using ValueType = typename PointType::value_type;

  NurbsCurve(size_t order, gsl::span<ValueType> knots,
             gsl::span<ControlPointType> controlPoints)
      : _order(order), _knots(knots.begin(), knots.end()),
        _controlPoints(controlPoints.begin(), controlPoints.end()) {
    Expects(order > 1);
    Expects(controlPoints.size() >= order);
    Expects(knots.size() == (controlPoints.size() + order));
  }

  PointType operator[](ValueType position) const {
    auto knotsBegin = _knots.begin();
    auto knotsEnd = _knots.end();
    auto spanStart = std::lower_bound(knotsBegin, knotsEnd, position);
    size_t controlPointIndex = std::distance(knotsBegin, spanStart);
    if (controlPointIndex == 0) {
      // Before the first knot. Just return the value of the first control
      // point.
      return _controlPoints.front().point;
    }

    if (spanStart == knotsEnd) {
      // We're past the last knot. Just return the value of the last control
      // point.
      return _controlPoints.back().point;
    }

    size_t controlPointsToProcess = _order;
    size_t controlPointCount = _controlPoints.size();
    if (controlPointIndex > controlPointCount) {
      controlPointsToProcess -= (controlPointIndex - controlPointCount);
    }

    PointType numerator{0};
    ValueType denominator{0};
    do {
      controlPointIndex--;
      auto &controlPoint = _controlPoints[controlPointIndex];
      ValueType weightedBasis =
          basisValue(position, controlPointIndex, _order) * controlPoint.weight;
      numerator += weightedBasis * _controlPoints[controlPointIndex].point;
      denominator += weightedBasis;
      controlPointsToProcess--;
    } while (controlPointIndex && controlPointsToProcess);
    return numerator / denominator;
  }

private:
  ValueType basisValue(ValueType position, size_t controlPointIndex,
                       size_t order) const {
    auto &controlPoint = _controlPoints[controlPointIndex];
    ValueType firstKnot = _knots[controlPointIndex];
    ValueType lastKnot = _knots[controlPointIndex + order];
    if (order == 2) {
      // Triangle function
      ValueType middleKnot = _knots[controlPointIndex + 1];
      if (position < middleKnot) {
        return rampUp(position, firstKnot, middleKnot);
      } else {
        return rampDown(position, middleKnot, lastKnot);
      }
    } else {
      size_t lowerOrder = order - 1;
      ValueType secondKnot = _knots[controlPointIndex + 1];
      ValueType secondToLastKnot = _knots[controlPointIndex + lowerOrder];

      ValueType result{0};
      if (position < secondToLastKnot) {
        result += rampUp(position, firstKnot, secondToLastKnot) *
                  basisValue(position, controlPointIndex, lowerOrder);
      }

      if (position > secondKnot) {
        result += rampDown(position, secondKnot, lastKnot) *
                  basisValue(position, controlPointIndex + 1, lowerOrder);
      }

      return result;
    }
  }

  ValueType rampUp(ValueType value, ValueType bottom, ValueType top) const {
    if (nearEqual(top, bottom)) {
      return 0.5;
    }
    return (value - bottom) / (top - bottom);
  }

  ValueType rampDown(ValueType value, ValueType bottom, ValueType top) const {
    if (nearEqual(top, bottom)) {
      return 0.5;
    }
    return (top - value) / (top - bottom);
  }

  size_t _order;
  std::vector<ValueType> _knots;
  std::vector<ControlPointType> _controlPoints;
};
} // namespace rev