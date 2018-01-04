#pragma once

#include "../lib/DataGroup.hpp"
#include "../lib/Util.hpp"

namespace datas {
namespace impl {

/*
 * A DataGroup backed by dense storage.
 * Only the first dimension may have its bounds adjusted.
 * Data is read and written such that the rightmost dimension is most densely packed.
 */
template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
class DataPointer : public lib::DataGroup<SampleType, FrameDimensionType, SubframeDimensionTypes...>
{
public:
	using Sample = SampleType;
	using DataGroup = lib::DataGroup<Sample, FrameDimensionType, SubframeDimensionTypes...>;
	using typename DataGroup::Dimensions;
	template <lib::util::Index index>
	using Dimension = typename DataGroup::template Dimension<index>;
	using DataGroup::dimensions;
	using Indices = lib::util::UniformTuple<lib::util::Index, 1 + sizeof...(SubframeDimensionTypes)>;

	DataPointer(Dimensions lowerBound, Indices count, Dimensions upperBound, Sample * initialData, lib::util::Index maxSamples);

	lib::util::Index available() const { return frameReserved - count.template get<0>(); }
	lib::util::Index reserved() const { return frameReserved; }

	/* will not allow more than reserved() total frames;
	 * return value is number actually grown
	 */
	lib::util::Index grow(lib::util::Index newFrames, Dimension<0> newUpperBound);

	void reseat(Sample * newData, lib::util::Index maxSamples);

	Sample * pointer() const { return back; }
	Sample * pointerTail() const { return pointer() + count.template get<0>() * samplesPerFrame(); }

	lib::util::Index samplesPerFrame() const;

	virtual Sample get(Dimensions at);

	virtual Sample & getIndex(Indices at);

	virtual Dimensions lowerBound() { return lower; }

	virtual Dimensions upperBound();

private:

	Sample * back;

	Dimensions const lower;
	Dimensions range;
	Indices count;
	//lib::util::Tuple<lib::util::Uniform<lib::util::Index, SubframeDimensionTypes...>> & subframesCount;
	//util::Tuple<lib::util::Uniform<lib::util::Index, subframeDimensionTypes...>> storageIndexs;
	//lib::util::Index frameCount;
	lib::util::Index frameReserved;
};

}
}

// impl

namespace datas {
namespace impl {

template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::DataPointer(Dimensions lowerBound, Indices count, Dimensions upperBound, Sample * initialData, lib::util::Index maxSamples)
: back(initialData),
  lower(lowerBound),
  range{lowerBound.map([](auto &lower, auto &upper)
	{
		return upper - lower;
	}, upperBound)},
  count{count}
{
	reseat(initialData, maxSamples);
	lib::util::Index frames = count.template get<0>();
	range.template get<0>() = 0;
	count.template get<0>() = 0;
	grow(count.template get<0>(), upperBound.template get<0>());
}

template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
void DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::reseat(Sample * newData, lib::util::Index maxSamples)
{
	back = newData;
	frameReserved = maxSamples / samplesPerFrame();
}


template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
lib::util::Index DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::samplesPerFrame() const
{
	return count.shift().accumulate([](auto &product, auto &next)
	{
		return product * next;
	});
}

template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
lib::util::Index DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::grow(lib::util::Index count, Dimension<0> newUpperBound)
{
	if (this->count.template get<0>() + count > frameReserved) {
		count = frameReserved;
	}

	std::get<0>(range) = newUpperBound - std::get<0>(lower);

	this->count.template get<0>() += count;
}

template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
SampleType DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::get(Dimensions at)
{
	Indices indices = lowerBound.map([](auto &lower, auto &upper, auto &count, auto &at)
	{
		return (at - lower) * count / (upper - lower);
	}, upperBound, count, at);

	return get(indices);
}

template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
SampleType & DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::getIndex(Indices at)
{
	/*
	 * coord = ((index3 + index2 * size3) + index1 * (size2 * size3)) + index0 * (size1*size2*size3)
	 * coord = index3 * 1 + index2 * 1 * size3 + index1 * 1 * size3 * size2 + index0 * 1 * size3 * size2 * size1
	 * coord = 1 * (index3 + size3 * (index2 + size2 * (index1 + size1 * (index0 + size0 * 0))))
	 */
	lib::util::Index coord = count.accumulateRight([](lib::util::Index previous, lib::util::Index index, lib::util::Index size)
	{
		return previous * size + index;
	}, 0, at);

	return back[coord];
}

template <typename SampleType, typename FrameDimensionType, typename... SubframeDimensionTypes>
auto DataPointer<SampleType, FrameDimensionType, SubframeDimensionTypes...>::upperBound() -> Dimensions
{
	return lower.map([](auto& lower, auto& range) {
		lower + range;
	}, range);
}

}
}
