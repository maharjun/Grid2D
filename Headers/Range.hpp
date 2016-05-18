#ifndef GRID2DTEST_RANGE_HPP
#define GRID2DTEST_RANGE_HPP

template <typename T>
struct Range {
	// represents a set of points {beginPoint...endPoint-1}
	T beginPoint;
	T endPoint;

	Range() : beginPoint(0), endPoint(0) {}

	Range(T beginPoint_, T endPoint_)
		: beginPoint(beginPoint_),
		  endPoint(endPoint_) {}

	inline bool contains(T val) const {
		return beginPoint <= val && endPoint > val;
	}
	inline bool contains(const Range &Interval) const {
		return beginPoint <= Interval.beginPoint && endPoint >= Interval.endPoint;
	}
	inline bool isBefore(T val) const {
		return endPoint <= val;
	}
	inline bool isBefore(const Range &Interval) const {
		return endPoint <= Interval.beginPoint;
	}
	inline bool isAfter(T val) const {
		return beginPoint > val;
	}
	inline bool isAfter(const Range &Interval) const {
		return beginPoint >= Interval.endPoint;
	}
	inline bool intersects(const Range &Interval) const {
		return !(isAfter(Interval) || isBefore(Interval));
	}
};

typedef Range<uint32_t> DiscreteRange;
typedef Range<float> SingleRange;
typedef Range<double> DoubleRange;

#endif //GRID2DTEST_RANGE_HPP
