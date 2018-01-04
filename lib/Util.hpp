#pragma once

#include <kvasir/mpl/mpl.hpp>

namespace datas {
namespace lib {
namespace util {

/* index/size type */
using Index = unsigned long;

/* A type list */
template <typename... ListedTypes>
using Types = kvasir::mpl::list<ListedTypes...>;

/* Element of a type list */
template <typename Types, Index index>
using Type = kvasir::mpl::at<kvasir::mpl::uint_<index>, Types>;

/* A tuple */
template <typename... ElementTypes>
class Tuple;
template <typename ElementType, typename... ElementTypes>
class Tuple<ElementType, ElementTypes...> : public Tuple<ElementTypes...>
{
private:
	using Base = Tuple<ElementTypes...>;

public:
	/* Constructor for each element */
	Tuple(ElementType element, ElementTypes... elements)
	: first(element), Base(elements...)
	{ }
		
	/* List of element types */
	using Types = util::Types<ElementType, ElementTypes...>;

	/* Type of an element */
	template <Index index>
	using Type = util::Type<Types, index>;

	/* number of elements */
	constexpr static Index size = sizeof...(ElementTypes) + 1;

	/* element access */
	template <Index index>
	Type<index> get()
	{
		if (index == 0)
			return first;
		return Base::template get<index - 1>();
	}

	/* mapping */
	template <typename F, typename... ExtraTuples>
	Tuple map(F const & f, ExtraTuples const &... extraTuples)
	{
		return { f(get<0>(), extraTuples.get<0>()...), Base::map(f, static_cast<typename ExtraTuples::Base>(extraTuples)...) };
	}

	/* accumulation / folding */
	template <typename F, typename... ExtraTuples>
	auto accumulateLeft(F const & f, decltype(f(ElementType(),ElementType())) initial, ExtraTuples const &... extraTuples)
	{
		return f(first, extraTuples.first..., Base::accumulateLeft(f, initial, static_cast<typename ExtraTuples::Base const &>(extraTuples)...));
	}
	template <typename F, typename... ExtraTuples>
	auto accumulateRight(F const & f, decltype(f(ElementType(),ElementType())) initial, ExtraTuples const &... extraTuples)
	{
		return Base::accumulateRight(f, f(initial, first, extraTuples.first...), static_cast<typename ExtraTuples::Base const &>(extraTuples)...);
	}

	Base & shift()
	{
		return *this;
	}

private:

	Tuple(ElementType first, Base const & base)
	: first(first), Base(base)
	{ }

	ElementType first;
};

/*
template <typename ElementType, typename... ElementTypes>
template <>
decltype(auto) & Tuple<ElementType, ElementTypes...>::get<0>()
{
	return first;
}
*/

template <>
class Tuple<>
{
	template <typename F, typename... ExtraTuples>
	auto accumulateLeft(F const & f, decltype(f({},{})) initial, ExtraTuples const &... extraTuples)
	{
		return initial;
	}
	template <typename F, typename... ExtraTuples>
	auto accumulateRight(F const & f, decltype(f({},{})) initial, ExtraTuples const &... extraTuples)
	{
		return initial;
	}
};

namespace detail {
	template <typename Type, Index size, typename... Types>
	struct uniform_tuple_deduction {
		using type = typename uniform_tuple_deduction<Type, size - 1, Type, Types...>::type;
	};
	template <typename Type, typename... Types>
	struct uniform_tuple_deduction<Type, 0, Types...> {
		using type = Tuple<Types...>;
	};
}

/* A tuple of all the same type */
template <typename Type, Index size>
using UniformTuple = typename detail::uniform_tuple_deduction<Type, size>::type;

}
}
}
