#ifndef LINEAR_REGION_HPP
#define LINEAR_REGION_HPP

#include <stdint.h>
#include <MexMemoryInterfacing/Headers/MexMem.hpp>

struct DiscreteInterval {
	// represents a set of points {beginPoint...endPoint-1}
	uint32_t beginPoint;
	uint32_t endPoint;

	DiscreteInterval() : beginPoint(0), endPoint(0) {}

	DiscreteInterval(uint32_t beginPoint_, uint32_t endPoint_)
		: beginPoint(beginPoint_),
		  endPoint(endPoint_) {}

	inline bool contains(uint32_t val) const {
		return beginPoint <= val && endPoint > val;
	}
	inline bool contains(const DiscreteInterval &Interval) const {
		return beginPoint <= Interval.beginPoint && endPoint >= Interval.endPoint;
	}
	inline bool isBefore(uint32_t val) const {
		return endPoint <= val;
	}
	inline bool isBefore(const DiscreteInterval &Interval) const {
		return endPoint <= Interval.beginPoint;
	}
	inline bool isAfter(uint32_t val) const {
		return beginPoint > val;
	}
	inline bool isAfter(const DiscreteInterval &Interval) const {
		return beginPoint >= Interval.endPoint;
	}
	inline bool intersects(const DiscreteInterval &Interval) const {
		return !(isAfter(Interval) || isBefore(Interval));
	}
};

class LinearRegion {
private:
	uint32_t regionLenLim;
	MexVector<DiscreteInterval> regionIntervals;

public:
	LinearRegion();
	explicit LinearRegion(uint32_t regionLenLim_);
	template<class Al>
	LinearRegion(uint32_t regionLenLim_, const MexVector<uint32_t, Al> &intervalBoundaryVect);
	
	inline uint32_t find(uint32_t val) const;
	inline void insert(uint32_t val);
	template<class Al>
	inline void assignVect(const MexVector<uint32_t, Al> &intervalBoundaryVect);
	inline void resize(uint32_t NewLenLim);
	inline const MexVector<DiscreteInterval> &getRegionIntervals() const;
	inline LinearRegion getDiff(const LinearRegion &otherLinRegion) const;
	inline bool contains(uint32_t val) const;
	inline MexVector<uint32_t> getValueVect() const;
};

#include "LinearRegion.inl"

#endif
