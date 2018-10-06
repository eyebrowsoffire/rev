#pragma once

#include <tuple>
#include <utility>

namespace rev
{

template <typename TupleType, typename VisitorType, size_t... indexes>
void visitTupleWithIndexes(TupleType &&tuple,
                           VisitorType &&visitor,
                           std::index_sequence<indexes...> &&)
{
    auto result = {(visitor(std::get<indexes>(tuple)), 0)...};
}

template <typename TupleType, typename VisitorType>
void visitTuple(TupleType &&tuple, VisitorType &&visitor)
{
    using Indexes = std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleType>>>;
    visitTupleWithIndexes(std::forward<TupleType>(tuple),
                          std::forward<VisitorType>(visitor),
                          Indexes{});
}

} // namespace rev