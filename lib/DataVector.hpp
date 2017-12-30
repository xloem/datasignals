#pragma once

#include <tuple>

namespace datas {
namespace lib {

/*
 * Generalizes the concept of a set of data, which all has the same dimension and the same type.
 * May be an ordered dense vector of sampled values over time,
 * or may be a sparse set of rare events from multiple spots in images taken from multiple devices at a different time each.
 *
 * Dimensions and Channels are std::tuples specifying the number and types of each property.
 */
template <typename Dimensions, typename Channels>
class DataVector
{
public:
	/* type of a given channel index */
	template <std::size_t channel>
	using Channel = typename std::tuple_element<channel, Channels>::type;

	/* a single data point */
	virtual Channels get(Dimensions at) = 0;

	class Iterator
	{
	public:
		virtual Iterator& ++operator() = 0;
		virtual Channels const & operator*() = 0;
		virtual bool operator !=(Iterator &) = 0;
	};

	Iterator begin();
	Iterator end();
};

/*
 * Do I want a datapoint class?
 * This class is basically a generalized map that can also describe a vector, in a more organized way.
 * Iterating through it is perhaps most usefully done across a dimension.
 */

}
}
