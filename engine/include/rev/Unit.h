#pragma once

#include "rev/IntegerSequenceUtilities.h"

namespace rev {

template <typename Enum, typename NumeratorCounts, typename DenominatorCounts>
struct UnitComposition;

template <typename... Components> struct UnitComponentEnumeration {
  static constexpr size_t kSize = sizeof...(Components);

  template <typename Type> static constexpr size_t kZero = 0;

  template <typename Component> struct SingleComponentComposition {
    using Numerator = std::integer_sequence<
        size_t, (std::is_same_v<Component, Components> ? 1 : 0)...>;
    using Denominator = std::integer_sequence<size_t, kZero<Components>...>;

    using Type =
        UnitComposition<UnitComponentEnumeration, Numerator, Denominator>;
  };

  template <typename Component>
  using MakeComposition = typename SingleComponentComposition<Component>::Type;
};

template <typename Enum, typename NumeratorCounts, typename DenominatorCounts>
struct UnitComposition {
  static_assert(Enum::kSize == NumeratorCounts::size());
  static_assert(Enum::kSize == DenominatorCounts::size());

  using Enumeration = Enum;
  using Numerator = NumeratorCounts;
  using Denominator = DenominatorCounts;
};

template <typename Composition>
using InvertComposition = UnitComposition<typename Composition::Enumeration,
                                          typename Composition::Denominator,
                                          typename Composition::Numerator>;

template <typename FirstComposition, typename SecondComposition>
struct CompositionMultiplier {
  static_assert(std::is_same_v<typename FirstComposition::Enumeration,
                               typename SecondComposition::Enumeration>);

  using CombinedNumerator =
      SumIntegerSeq<typename FirstComposition::Numerator,
                    typename SecondComposition::Numerator>;
  using CombinedDenominator =
      SumIntegerSeq<typename FirstComposition::Denominator,
                    typename SecondComposition::Denominator>;

  using CanceledComponents =
      MinIntegerSeq<CombinedNumerator, CombinedDenominator>;

  using ReducedNumerator =
      SubtractIntegerSeq<CombinedNumerator, CanceledComponents>;
  using ReducedDenominator =
      SubtractIntegerSeq<CombinedDenominator, CanceledComponents>;

  using Type = UnitComposition<typename FirstComposition::Enumeration,
                               ReducedNumerator, ReducedDenominator>;
};

template <typename FirstComposition, typename SecondComposition>
using MultiplyComposition =
    typename CompositionMultiplier<FirstComposition, SecondComposition>::Type;

template <typename ValueType, typename Composition> class Unit {
public:
  Unit() : _value(0) {}

  template <typename... Args, typename = std::enable_if_t<
                                  std::is_constructible_v<ValueType, Args...>>>
  Unit(Args &&... args) : _value(std::forward<Args>(args)...) {}

  const ValueType &getValue() const { return _value; }

  template <typename OtherValueType, typename OtherComposition>
  auto operator*(const Unit<OtherValueType, OtherComposition> &other) const {
    using NewCompositionType =
        MultiplyComposition<Composition, OtherComposition>;
    auto newValue = _value * other.getValue();

    return Unit<decltype(newValue), NewCompositionType>(newValue);
  }

  template <typename OtherValueType, typename OtherComposition>
  auto operator/(const Unit<OtherValueType, OtherComposition> &other) const {
    using NewCompositionType =
        MultiplyComposition<Composition, InvertComposition<OtherComposition>>;
    auto newValue = _value / other.getValue();

    return Unit<decltype(newValue), NewCompositionType>(newValue);
  }

  Unit operator+(const Unit &other) const {
    return Unit(_value + other.getValue());
  }

  Unit operator-(const Unit &other) const {
    return Unit(_value - other.getValue());
  }

  Unit &operator+=(const Unit &other) {
    _value += other.getValue();
    return *this;
  }

  Unit &operator-=(const Unit &other) {
    _value -= other.getValue();
    return *this;
  }

private:
  ValueType _value;
};

} // namespace rev