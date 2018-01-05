#pragma once

//#include <kvasir/mpl/mpl.hpp>
//#include <brigand/sequences/list.hpp>
//#include <brigand/sequences/at.hpp>
//#include <tuple>
#include <boost/hana.hpp>

namespace datas {
namespace lib {
namespace util {

/* index/size type */
using Index = unsigned long;

/* A type list */
template <typename... ListedTypes>
//using Types = kvasir::mpl::list<ListedTypes...>;
//using Types = brigand::list<ListedTypes...>;
using Types = boost::hana::basic_tuple<boost::hana::type<ListedTypes>...>;

/* Element of a type list */
template <typename Types, Index index>
//using Type = kvasir::mpl::at<kvasir::mpl::uint_<index>, Types>;
//using Type = brigand::at_c<Types, index>;
using Type = typename decltype(+boost::hana::at(Types(), boost::hana::size_c<index>))::type;

/* A tuple */
template <typename... ElementTypes>
class Tuple;
template <typename ElementType, typename... ElementTypes>
class Tuple<ElementType, ElementTypes...>
{
public:
	/* Constructor for each element */
	Tuple(ElementType && element, ElementTypes &&... elements)
	: _back(element, elements...)
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
	Type<index> & get() {
		return boost::hana::at(_back, boost::hana::size_c<index>);
	}
	template <Index index>
	Type<index> const & get() const {
		return boost::hana::at(_back, boost::hana::size_c<index>);
	}

	/* mapping */
	template <typename F, typename... ExtraTuples>
	Tuple map(F const & f, ExtraTuples const &... extraTuples) const
	{
		return _tuple_as_Tuple(boost::hana::zip_with(f, _back, extraTuples._back...));
	}

	/* accumulation / folding */
	template <typename F, typename Initial, typename... ExtraTuples> const
	auto foldl(F const & f, Initial const & initial, ExtraTuples const &... extraTuples)
	{
		return boost::hana::fold_left(boost::hana::zip(_back, extraTuples._back...), initial, [&f](auto && state, auto && vals){
			return boost::hana::unpack(boost::hana::insert(vals, boost::hana::size_c<size>, state), f);
		});
	}
	template <typename F, typename Initial, typename... ExtraTuples> const
	auto foldr(F const & f, Initial const & initial, ExtraTuples const &... extraTuples)
	{
		return boost::hana::fold_right(boost::hana::zip(_back, extraTuples._back...), initial, [&f](auto && vals, auto && state) {
			return boost::hana::unpack(boost::hana::insert(vals, boost::hana::size_c<0>, state), f);
		});
	}

	Tuple<ElementTypes...> shift() const
	{
		return Tuple<ElementTypes...>::_tuple_as_Tuple(boost::hana::take_back(_back, boost::hana::size_c<size - 1>));
	}

	static Tuple _tuple_as_Tuple(boost::hana::basic_tuple<ElementType, ElementTypes...> && tuple)
	{
		return *reinterpret_cast<Tuple*>(&tuple);
	}

	boost::hana::basic_tuple<ElementType, ElementTypes...> _back;
};

#if 0
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
	template <Index index> Type<index> & get();
	template <Index index> Type<index> const & get() const;

	/* mapping */
	template <typename F, typename... ExtraTuples>
	Tuple map(F const & f, ExtraTuples const &... extraTuples)
	{
		return { f(get<0>(), extraTuples.template get<0>()...), Base::map(f, static_cast<typename ExtraTuples::Base>(extraTuples)...) };
	}

	/* accumulation / folding */
	template <typename F, typename... ExtraTuples>
	auto foldl(F const & f, decltype(f(ElementType(),ElementType())) initial, ExtraTuples const &... extraTuples)
	{
		return f(first, extraTuples.first..., Base::foldl(f, initial, static_cast<typename ExtraTuples::Base const &>(extraTuples)...));
	}
	template <typename F, typename... ExtraTuples>
	auto foldr(F const & f, decltype(f(ElementType(),ElementType())) initial, ExtraTuples const &... extraTuples)
	{
		return Base::foldr(f, f(initial, first, extraTuples.first...), static_cast<typename ExtraTuples::Base const &>(extraTuples)...);
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

namespace detail {
	template <Index index, typename Tuple>
	struct TupleGet {
		static typename Tuple::template Type<index> & get(Tuple &tuple)
		{
			return get<index - 1>(tuple->shift());
		}
	};
	template <typename Tuple>
	struct TupleGet<0, Tuple> {
		static typename Tuple::template Type<0> & get(Tuple &tuple)
		{
			return tuple.first;
		}
	};
}

template <typename ElementType, typename ...ElementTypes>
template <Index index>
Tuple<ElementType, ElementTypes...>::Type<index> & Tuple<ElementType, ElementTypes...>::get()
{
	return detail::TupleGet<0,Tuple<ElementType, ElementTypes...>>::get(*this);
}
#endif

template <>
class Tuple<>
{
	template <typename F, typename... ExtraTuples>
	auto foldl(F const & f, decltype(f({},{})) initial, ExtraTuples const &... extraTuples)
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
