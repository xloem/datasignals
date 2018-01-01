#pragma once

#include <tuple>

namespace datas {
namespace lib {

/*
 * Generalizes the concept of a set of data, which all has the same dimension and the same type.
 * May be an ordered dense vector of sampled values over time,
 * or may be a sparse set of rare events from multiple spots in images taken from multiple devices at a different time each.
 */
template <typename _Sample, typename ..._Dimensions>
class DataVector
{
public:
	/* type of a single sample */
	using Sample = _Sample;

	/* type of Dimensions */
	using Dimensions = std::tuple<_Dimensions...>;

	/* type of a given dimension index */
	template <std::size_t dimension>
	using Dimension = typename std::tuple_element<dimension, Dimensions>::type;

	/* number of dimensions */
	static constexpr std::size_t dimensions = sizeof...(_Dimensions);

	/* a single data point */
	virtual Sample get(Dimensions at) = 0;

	/* smallest coordinates present in data */
	virtual Dimensions lowerBound() = 0;

	/* largest coordinates present in data */
	virtual Dimensions upperBound() = 0;
};

/*
 * Do I want a datapoint class?
 * This class is basically a generalized map that can also describe a vector, in a more organized way.
 * Iterating through it is perhaps most usefully done across a dimension.
 */

}
}
