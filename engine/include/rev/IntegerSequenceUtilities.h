#pragma once

#include <algorithm>
#include <utility>

namespace rev {
template <typename IntegerSeq> struct IntegerGetter;

template <size_t... integers>
struct IntegerGetter<std::index_sequence<integers...>> {
  template <size_t index, size_t firstInteger, size_t... remainingIntegers>
  struct GetAtIndex {
    static constexpr size_t value =
        GetAtIndex<index - 1, remainingIntegers...>::value;
  };

  template <size_t firstInteger, size_t... remainingIntegers>
  struct GetAtIndex<0, firstInteger, remainingIntegers...> {
    static constexpr size_t value = firstInteger;
  };

  template <size_t index>
  static constexpr size_t Get = GetAtIndex<index, integers...>::value;
};

template <template <size_t, size_t> typename Function, typename First,
          typename Second>
struct ZipIntegerSeq {
  static_assert(First::size() == Second::size(),
                "Integer sequence size mismatch.");

  using FirstGetter = IntegerGetter<First>;
  using SecondGetter = IntegerGetter<Second>;

  template <typename Indexes> struct Zip;

  template <size_t... indexes>
  struct Zip<std::integer_sequence<size_t, indexes...>> {
    using Type = std::integer_sequence<
        size_t, (Function<FirstGetter::template Get<indexes>,
                          SecondGetter::template Get<indexes>>::value)...>;
  };

  using Type = typename Zip<std::make_index_sequence<First::size()>>::Type;
};

template <size_t first, size_t second> struct SumIntegerFunction {
  static constexpr size_t value = first + second;
};

template <typename First, typename Second>
using SumIntegerSeq =
    typename ZipIntegerSeq<SumIntegerFunction, First, Second>::Type;

template <size_t first, size_t second> struct SubtractIntegerFunction {
  static_assert(first >= second);
  static constexpr size_t value = first - second;
};

template <typename First, typename Second>
using SubtractIntegerSeq =
    typename ZipIntegerSeq<SubtractIntegerFunction, First, Second>::Type;

template <size_t first, size_t second> struct MinIntegerFunction {
  static constexpr size_t value = std::min(first, second);
};

template <typename First, typename Second>
using MinIntegerSeq =
    typename ZipIntegerSeq<MinIntegerFunction, First, Second>::Type;

template <typename First, typename Second> struct MinIndexSeq {};

}; // namespace rev