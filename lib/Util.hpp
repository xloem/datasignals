#pragma once

#include <tuple>
#include <boost/hana.hpp>
#include <boost/hana/ext/std/tuple.hpp>

namespace datas {
namespace lib {
namespace util {

template <typename F, typename... Tt>
auto elementwise(Tt const &... a, F const & f)
{
	return boost::hana::zip_with(f, a...);
}

template <typename F, typename T, typename... Ts>
T accumulate(std::tuple<T, Ts...> const & a, F const & f)
{
	return boost::hana::fold_left(a, f);
}


}
}
}
