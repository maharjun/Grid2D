#ifndef LINEAR_REGION_HPP
#define LINEAR_REGION_HPP

#include <stdint.h>
#include "Range.hpp"
#include <MexMemoryInterfacing/Headers/MexMem.hpp>

template <class Al=mxAllocator>
class LinearRegion {
private:
	uint32_t regionLenLim;
	MexVector<DiscreteRange, Al> regionIntervals;

public:
	LinearRegion();
	explicit LinearRegion(uint32_t regionLenLim_);
	template<class Al2>
	LinearRegion(uint32_t regionLenLim_, const MexVector<uint32_t, Al2> &intervalBoundaryVect);
	
	inline uint32_t find(uint32_t val) const;
	inline void insert(uint32_t val);
	template<class Al2>
	inline void assignVect(const MexVector<uint32_t, Al2> &intervalBoundaryVect);
	inline void resize(uint32_t NewLenLim);
	inline const MexVector<DiscreteRange, Al> &getRegionIntervals() const;
	inline LinearRegion<Al> getDiff(const LinearRegion &otherLinRegion) const;
	inline bool contains(uint32_t val) const;
	inline MexVector<uint32_t> getValueVect() const;
};

#include "LinearRegion.inl"

#endif
