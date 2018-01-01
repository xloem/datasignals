#pragma once

#include "../lib/DataVector.hpp"

#include <utility>
#include <vector>


namespace datas {
namespace impl {

/*
 * A DataVector backed by dense storage.
 * Only the first dimension may have its bounds adjusted.
 * Data is read and written such that the rightmost dimension is most densely packed.
 */
template <typename Sample, typename... _Dimensions>
class DataArray : public lib::DataVector<Sample, _Dimensions...>
{
public:
	using DataVector = lib::DataVector<Sample, _Dimensions...>;
	using typename DataVector::Dimensions;
	using DataVector::dimensions;
	template <std::size_t dimension>
	using Dimension = typename DataVector::template Dimension<dimension>;
	using Indices = std::array<std::size_t, dimensions>;

	DataArray(Dimensions lowerBound, Indices count, Dimensions upperBound, Sample * initialData = NULL);

	std::size_t samplesPerFrame() const;

	void append(std::size_t count, Dimension<0> newUpperBound, Sample * data);

	virtual Sample get(Dimensions at);

	virtual Sample get(Indices at) const;

	virtual Dimensions lowerBound() { return lower; }

	virtual Dimensions upperBound();

private:
	std::vector<Sample> back;

	Dimensions const lower;
	Dimensions range;
	Indices count;

};

}
}

// impl
#include "../lib/Util.hpp"

#include <cassert>

template <typename Sample, typename... _Dimensions>
datas::impl::DataArray<Sample, _Dimensions...>::DataArray(Dimensions lowerBound, std::array<std::size_t, dimensions> count, Dimensions upperBound, Sample * initialData)
: lower(lowerBound),
  range{datas::lib::util::elementwise(lowerBound, upperBound, [](auto &lower, auto &upper)
	{
		return upper - lower;
	})},
  count(count)
{
	std::get<0>(range) = 0;
	this->count[0] = 0;

	append(count[0], std::get<0>(upperBound), initialData);
}

template <typename Sample, typename... _Dimensions>
std::size_t datas::impl::DataArray<Sample, _Dimensions...>::samplesPerFrame() const
{
	std::size_t product = 1;
	for (std::size_t i = 1; i < dimensions; ++ i)
		product *= count[i];
	return product;
}

template <typename Sample, typename... _Dimensions>
void datas::impl::DataArray<Sample, _Dimensions...>::append(std::size_t count, Dimension<0> newUpperBound, Sample * data)
{
	std::size_t size = count * samplesPerFrame();

	back.insert(back.end(), data, data + size);
	std::get<0>(range) = newUpperBound - std::get<0>(lower);

	this->count[0] += count;

	assert(this->count[0] * size == back.size());
}

template <typename Sample, typename... _Dimensions>
Sample datas::impl::DataArray<Sample, _Dimensions...>::get(Dimensions at)
{
	Indices indices = datas::lib::util::elementwise(lowerBound, upperBound, count, at, [](auto &lower, auto &upper, auto &count, auto &at)
		{
			return (at - lower) * count / (upper - lower);
		});

	return get(indices);
}

template <typename Sample, typename... _Dimensions>
Sample datas::impl::DataArray<Sample, _Dimensions...>::get(Indices at) const
{
	std::size_t index = 0;
	for (std::size_t i = 0; i < dimensions; ++ i) {
		index *= count[i];
		index += at[i];
	}

	return back[index];
}

template <typename Sample, typename... _Dimensions>
auto datas::impl::DataArray<Sample, _Dimensions...>::upperBound() -> Dimensions
{
	return datas::lib::util::elementwise(lower, range, [](auto& lower, auto& range) {
		lower + range;
	});
}
