#pragma once

#include "Util.hpp"

namespace datas {
namespace lib {

/*
 * Generalizes the concept of a set of data, which all has the same dimension and the same type.
 * May be an ordered dense vector of sampled values over time,
 * or may be a sparse set of rare events from multiple spots in images taken from multiple devices at a different time each.
 */
template <typename SampleType, typename ...DimensionTypes>
class DataGroup
{
public:
	/* type of a single sample */
	using Sample = SampleType;

	/* Tuple of dimension types */
	using Dimensions = util::Tuple<DimensionTypes...>;

	/* type of a given dimension */
	template <util::Index index>
	using Dimension = typename Dimensions::template Type<index>;

	/* number of dimensions */
	static constexpr util::Index dimensions = sizeof...(DimensionTypes);

	/* a single data point */
	virtual Sample get(Dimensions) = 0;

	/* smallest coordinates present in data */
	virtual Dimensions lowerBound() = 0;

	/* largest coordinates present in data */
	virtual Dimensions upperBound() = 0;
};

/*
 * This class is basically a generalized map that can also describe a vector, in a more organized way.
 * Iterating through it is perhaps most usefully done across a dimension.
 */

}
}
