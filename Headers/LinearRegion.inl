#ifndef REGIONCOLUMN_INL
#define REGIONCOLUMN_INL

#include <limits.h>
#include <stdint.h>
#include <MexMemoryInterfacing/Headers/MexMem.hpp>
#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include "RegionExc.hpp"
#include "LinearRegion.hpp"

// ##################################################################
// CONSTRUCTORS
// ##################################################################

inline LinearRegion::LinearRegion() : regionLenLim(UINT32_MAX), regionIntervals() {}

inline LinearRegion::LinearRegion(uint32_t regionLenLim_) : regionLenLim(regionLenLim_), regionIntervals() {}

template <class Al>
inline LinearRegion::LinearRegion(uint32_t regionLenLim_, const MexVector<uint32_t, Al> &intervalBoundaryVect) : LinearRegion(regionLenLim_) {
	this->assignVect(intervalBoundaryVect);
}

// ##################################################################
// MEMBER FUNCTIONS
// ##################################################################

uint32_t LinearRegion::find(uint32_t val) const {
	/*
	 * This function returns the index i such that the value val is either con-
	 * tained in the interval regionIntervals[i] or else, regionIntervals[i]
	 * is the first interval after val.
	 */

	// If debug, validate said point
	if (IS_DEBUG && val >= this->regionLenLim) {
		throw RegionException::INDEX_OUT_OF_LIMS;
	}

	// Search for this point in the given column
	bool isInsideInterval = false;
	bool isBeforeInterval = false;
	uint32_t IntervalIndex = 0;

	for(uint32_t i=0; i<regionIntervals.size(); ++i) {
		if (regionIntervals[i].contains(val)) {
			isInsideInterval = true;
			IntervalIndex = i;
		}
		else if(regionIntervals[i].isAfter(val)) {
			isBeforeInterval = true;
			IntervalIndex = i;
			break;
		}
	}

	if(!isInsideInterval && !isBeforeInterval) {
		// This means that the given y coordinate lies beyond any of the
		// currently contiguous regions. This means that we return the
		// length of regionIntervals
		IntervalIndex = regionIntervals.size();
	}
	return IntervalIndex;
}

void LinearRegion::insert(uint32_t val) {
	/* 
	 * This function inserts the value val into the linear region.
	 * 
	 * It first checks for the contiguous section that either contains it
	 * or is the first section to be ahead of it. Then, if it is not con-
	 * tained in any existing section, it is added according to the foll-
	 * owing rules:
	 * 
	 * 1.  If it is adjacent to a single section  (i.e. either to the one 
	 *     before or after it,  the section that it is adjacent to is ex-
	 *     tended.
	 * 
	 * 2.  If it is the 'missing link' between two sections, the sections
	 *     are fused.
	 * 
	 * 3.  If it is not adjacent  to any section,  a new section containg
	 *     only this point is created.
	 */
	uint32_t IntervalIndex = this->find(val);
	if (IntervalIndex == regionIntervals.size()
	    || regionIntervals[IntervalIndex].isAfter(val)) {
		// The above condition means that the point needs to be inserted
		// and is not already present

		bool isJustBeforeNextSeg = false;
		bool isJustAfterPrevSeg = false;

		if (!regionIntervals.isempty()) {
			if (IntervalIndex > 0)
				isJustAfterPrevSeg = (regionIntervals[IntervalIndex-1].endPoint == val);
			if (IntervalIndex < regionIntervals.size())
				isJustBeforeNextSeg = (regionIntervals[IntervalIndex].beginPoint == val + 1);
		}

		if (isJustAfterPrevSeg && !isJustBeforeNextSeg) {
			regionIntervals[IntervalIndex-1].endPoint++;
		}
		else if (isJustBeforeNextSeg && !isJustAfterPrevSeg) {
			regionIntervals[IntervalIndex].beginPoint--;
		}
		else if (isJustBeforeNextSeg && isJustAfterPrevSeg) {
			// in this case we need to fuse two regions
			regionIntervals[IntervalIndex - 1].endPoint = regionIntervals[IntervalIndex].endPoint;
			regionIntervals.erase(IntervalIndex);
		}
		else{
			// In this case we need to add a new region.
			regionIntervals.insert(IntervalIndex, DiscreteRange(val, val + 1));
		}
	}
}

void LinearRegion::resize(uint32_t NewLenLim)
{
	if (NewLenLim < this->regionLenLim) {
		// Will have to strip the points off
		// Calculate position of new last point in current region
		auto IntervalIndex = find(NewLenLim-1);
		uint32_t TerminationSize;

		// Find if the current point exists in interval
		if (regionIntervals[IntervalIndex].contains(NewLenLim - 1)) {
			// in this case, the segment [SB[2I], SB[2I+1]) is partially
			// cutoff in the middle by NewLenLim and it now extends from
			// [SB[2I], NewLenLim-1] == [SB[2I], NewLenLim)
			regionIntervals[IntervalIndex].endPoint = NewLenLim;
			TerminationSize = IntervalIndex + 1;
		}
		else {
			// In this case, the last point does not belong to any segment
			// Thus, the IntervalIndex returned points to an interval beyond
			// the new allowed range. hence this interval and all subsequent
			// intervals are removed
			TerminationSize = IntervalIndex;
		}
		regionIntervals.resize(TerminationSize);
	}
	this->regionLenLim = NewLenLim;
}

inline const MexVector<DiscreteRange> & LinearRegion::getRegionIntervals() const
{
	return regionIntervals;
}

inline LinearRegion LinearRegion::getDiff(const LinearRegion & otherLinRegion) const
{
	/* 
	 * This function returns the  difference of the  two Linear Regions.  For
	 * example, A.getDiff(B) returns a Region  containing all the values in A
	 * which are not in B.
	 */
	/* 
	 * Implemented as state machine.
	 * 
	 * State variables:
	 * 
	 *     ACursor - A cursor That points to a potential start of an interval
	 *			     in the difference
	 *     BInterval - The interval in B.
	 *     
	 *     This is a derived state
	 *     AInterval - The interval in A inside which ACursor is contained
	 * 
	 * while BInterval and AInterval are valid:
	 *     if BInterval < ACursor:
	 *         increment BInterval
	 *     else if BInterval cont ACursor:
	 *         if BInterval cont AInterval:
	 *             increment AInterval
	 *             ACursor = AInterval.begin
	 *         else:
	 *             ACursor = BInterval.end
	 *     else if BInterval > ACursor:
	 *         if AInterval cont BInterval.begin():
	 *             add [AInterval.begin, BInterval.begin) to diff
	 *             ACursor = BInterval.begin
	 *         else:
	 *             add AInterval to diff
	 *             increment AInterval
	 *             ACursor = AInterval.begin
	 */

	if (regionIntervals.isempty()) {
		return LinearRegion(this->regionLenLim);
	}

	LinearRegion DiffRegion(this->regionLenLim);
	auto &ASegBounds = this->getRegionIntervals();
	auto &BSegBounds = otherLinRegion.getRegionIntervals();

	uint32_t AIntervalIndex = 0; 
	uint32_t BIntervalIndex = 0; 
	uint32_t ACursor = ASegBounds[AIntervalIndex].beginPoint;

	while(
		AIntervalIndex < ASegBounds.size() &&
		BIntervalIndex < BSegBounds.size()) {

		auto AInterval = ASegBounds[AIntervalIndex];
		auto BInterval = BSegBounds[BIntervalIndex];

		// if BInterval < ACursor
		if(BInterval.isBefore(ACursor)) {
			BIntervalIndex++;
		}
		// else if BInterval Cont ACursor
		else if (BInterval.contains(ACursor)) {
			if(BInterval.contains(AInterval)) {
				AIntervalIndex++;
				ACursor = (AIntervalIndex < ASegBounds.size()) ? ASegBounds[AIntervalIndex].beginPoint : 0;
			}
			else {
				ACursor = BSegBounds[BIntervalIndex].endPoint;
			}
		}
		// else if BInterval > ACursor
		else {
			if(AInterval.intersects(BInterval)) {
				DiffRegion.regionIntervals.push_back(DiscreteRange(ACursor, BInterval.beginPoint));
				ACursor = BInterval.beginPoint;
			}
			else {
				DiffRegion.regionIntervals.push_back(DiscreteRange(ACursor, AInterval.endPoint));
				AIntervalIndex++;
				ACursor = (AIntervalIndex < ASegBounds.size()) ? ASegBounds[AIntervalIndex].beginPoint : 0;
			}
		}
	}

	if (AIntervalIndex < ASegBounds.size()) {
		DiffRegion.regionIntervals.push_back(DiscreteRange(ACursor, ASegBounds[AIntervalIndex].endPoint));
		AIntervalIndex++;
		for (; AIntervalIndex < ASegBounds.size(); ++AIntervalIndex) {
			DiffRegion.regionIntervals.push_back(ASegBounds[AIntervalIndex]);
		}
	}

	return DiffRegion;
}

inline MexVector<uint32_t> LinearRegion::getValueVect() const
{
	/* This function  returns a vector  containing the values  in the current
	 * region. The vector is sorted in ascending order.
	 */
	MexVector<uint32_t> ValueVect;
	for(auto &Interval : this->regionIntervals) {
		for (auto i = Interval.beginPoint; i < Interval.endPoint; ++i) {
			ValueVect.push_back(i);
		}
	}
	return ValueVect;
}

inline bool LinearRegion::contains(uint32_t val) const {
	auto IntervalIndex = this->find(val);
	return (regionIntervals[IntervalIndex].contains(val));
}

template <class Al>
void LinearRegion::assignVect(const MexVector<uint32_t, Al> &intervalBoundaryVect) {
	// Validate intervalBoundaryVect and assign.

	// Check if intervalBoundaryVect is of even length
	bool isVectValid = !(intervalBoundaryVect.size()%2);;
	if (isVectValid) {
		// Check if intervalBoundaryVect is sorted in strictly ascending order.
		// and if all elements except the last are within range
		for(size_t i = 0; i+1 < intervalBoundaryVect.size(); ++i) {
			if (intervalBoundaryVect[i] >= intervalBoundaryVect[i+1] || intervalBoundaryVect[i] >= regionLenLim) {
				isVectValid = false;
				break;
			}
		}
	}
	if (isVectValid && !intervalBoundaryVect.isempty()) {
		// Check if last element is within range or == length
		isVectValid = (intervalBoundaryVect.last() <= regionLenLim);
	}

	if (isVectValid) {
		regionIntervals.resize(intervalBoundaryVect.size()/2);
		for(int i=0; i<intervalBoundaryVect.size(); i+=2) {
			regionIntervals.push_back(DiscreteRange(intervalBoundaryVect[i], intervalBoundaryVect[i+1]));
		}
	}
	else {
		WriteException(RegionException::INVALID_LINEAR_REGION, "The Given Linear Region Is Invalid\n");
	}
}


#endif
