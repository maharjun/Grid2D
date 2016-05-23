#ifndef REGION_HPP
#define REGION_HPP

#include <stdint.h>
#include <type_traits>
#include <algorithm>

#include <MexMemoryInterfacing/Headers/MexMem.hpp>
#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include "RegionExc.hpp"
#include "Point.hpp"
#include "RowColumnRegion.hpp"

struct Region{
private:
	uint32_t gridXLim;
	uint32_t gridYLim;
	mutable MexVector<ColumnRegion> gridColumns;
	mutable MexVector<RowRegion> gridRows;
	mutable PointSet nonAddedPoints;
	mutable PointSet innerBoundary;
	
	// The below are status variables used for lazy updating
	mutable bool isRowColUpdated;
	mutable bool isInnerBoundaryUpdated;

	inline void initGridRowColumns() {
		gridColumns.resize(gridXLim);
		gridRows.resize(gridYLim);
		for (uint32_t x = 0; x < gridXLim; ++x) {
			gridColumns[x] = ColumnRegion(x, gridYLim);
		}
		for (uint32_t y = 0; y < gridYLim; ++y) {
			gridRows[y] = RowRegion(y, gridXLim);
		}
	}

	void addNonAddedPoints() const;
	void updateInnerBoundary() const;
	
public:
	
	Region() : 
		gridXLim               (0),
		gridYLim               (0), 
		gridColumns            (),
		gridRows               (), 
		nonAddedPoints         (),
		innerBoundary          (),
		isRowColUpdated        (true),
		isInnerBoundaryUpdated (true) {}

	Region(uint32_t gridXLim_, uint32_t gridYLim_) : Region() {
		gridXLim    = gridXLim_;
		gridYLim    = gridYLim_;
		this->initGridRowColumns();
	}

	Region(PointMexVect &Boundary, uint32_t gridXLim_, uint32_t gridYLim_) : Region(gridXLim_, gridYLim_) {
		/*
		 * This function is used to initialize a Region through a Midway Boundary.
		 *
		 * First, We bucket the points according to their X coordinates. Then we sort
		 * the buckets. These buckets become the gridColumns
		 *
		 * Next, We bucket the points according to their Y coordinates. Then we sort
		 * the buckets. These buckets become the gridRows.
		 *
		 */

		// CAllocator is used here due to matlabs horrible inefficiency in cleaning up
		// memory.
		MexVector<MexVector<uint32_t, CAllocator>> gridColumnBuckets(gridXLim_);
		MexVector<MexVector<uint32_t, CAllocator>> gridRowBuckets(gridYLim_);

		// Validating Boundary
		// 1.  Every point must be adjacent to the next
		// 2.  All points should be <= gridlims
		bool isBoundaryValid = true;
		for(int i=0; i<Boundary.size(); ++i) {
			auto CurrentPoint = Boundary[i];
			auto NextPoint = (i+1<Boundary.size())?Boundary[i+1]:Boundary[0];

			if (CurrentPoint.x == NextPoint.x &&
				(CurrentPoint.y + 1 == NextPoint.y || CurrentPoint.y == NextPoint.y + 1)) {}
			else if(CurrentPoint.y == NextPoint.y &&
				(CurrentPoint.x + 1 == NextPoint.x || CurrentPoint.x == NextPoint.x + 1)) {}
			else {
				isBoundaryValid = false;
				break;
			}
			if (Boundary[i].x > gridXLim_ || Boundary[i].y > gridYLim_) {
				isBoundaryValid = false;
				break;
			}
		}
		if(isBoundaryValid) {
			isBoundaryValid = (
				(Boundary.last().x == Boundary[0].x || Boundary.last().y == Boundary[0].y) &&
				Boundary.last().x <= gridXLim_ && Boundary.last().y <= gridYLim_
			);
		}

		if(isBoundaryValid) {
			// Bucketing points
			for(int i=0; i<Boundary.size(); ++i) {

				auto CurrentPoint = Boundary[i];
				auto NextPoint = (i+1<Boundary.size())?Boundary[i+1]:Boundary[0];

				if(NextPoint.x == CurrentPoint.x && NextPoint.y == CurrentPoint.y+1) {
					// Upward edge => left edge => insert point into row
					gridRowBuckets[CurrentPoint.y].push_back(CurrentPoint.x);
				}
				else if(NextPoint.x == CurrentPoint.x && NextPoint.y+1 == CurrentPoint.y) {
					// Downward edge => right edge => insert point into row.
					gridRowBuckets[CurrentPoint.y-1].push_back(CurrentPoint.x);
				}
				else if(NextPoint.x == CurrentPoint.x+1 && NextPoint.y == CurrentPoint.y) {
					// Rightward edge => Top edge => insert point into column.
					gridColumnBuckets[CurrentPoint.x].push_back(CurrentPoint.y);
				}
				else if(NextPoint.x+1 == CurrentPoint.x && NextPoint.y == CurrentPoint.y) {
					// Leftward edge => Bottom edge => insert point into column.
					gridColumnBuckets[CurrentPoint.x-1].push_back(CurrentPoint.y);
				}
			}

			// Sorting
			for(auto &column : gridColumnBuckets) {
				std::sort(column.begin(), column.end());
			}
			for(auto &row : gridRowBuckets) {
				std::sort(row.begin(), row.end());
			}

			// Assigning
			for(int i=0; i<gridXLim_; ++i) {
				gridColumns[i].assignVect(gridColumnBuckets[i]);
			}
			for(int i=0; i<gridYLim_; ++i) {
				gridRows[i].assignVect(gridRowBuckets[i]);
			}
		}
		else {
			WriteException(RegionException::INVALID_BOUNDARY, "The Boundary specified is invalid\n");
		}
	}
	void consolidate() const;
	inline void insert(const Point& point2Insert) {
		this->nonAddedPoints.insert(point2Insert);
		this->isRowColUpdated = false;
		this->isInnerBoundaryUpdated = false;
	}
	inline bool contains(const Point& point2Search) const {
		bool isContained = nonAddedPoints.count(point2Search);
		if (!isContained) {
			auto contIntervalIndex = gridColumns[point2Search.x].find(point2Search);
			if(contIntervalIndex < gridColumns[point2Search.x].getRegionIntervals().size()){
				auto &containingInterval = gridColumns[point2Search.x].getRegionIntervals()[contIntervalIndex];
				isContained = containingInterval.contains(point2Search.y);
			}
		}
		return isContained;
	}
	template <typename T, typename B=typename std::enable_if<std::is_floating_point<T>::value>::type>
	inline bool contains(const GenericPoint<T>& contPoint2Search) const {
		Point closestPoint  = Point(contPoint2Search.x+0.5, contPoint2Search.y+0.5);

		return contains(closestPoint);
	}
	inline const PointSet & getInnerBoundary() const {
		consolidate();
		return this->innerBoundary;
	}
	PointSet getOuterBoundary() const;
	PointMexVect getMidwayBoundary() const;

	/*
	 * This describes the algorithm in detail.
	 *
	 * Every Region has 3 boundaries. The boundary is either an inner
	 * boundary,  outer boundary, or edge boundary.  They are defined 
	 * below:
	 * 
	 * Inner Boundary -
	 * 
	 *   This boundary is the set of points in the regionfor which at
	 *   least one of the  horiz/vert (HV) neighbors  does not lie in 
	 *   the region.  This boundary is a property  (i.e. data member)
	 *   of the class and is updated lazily upon get.
	 *   
	 * Outer Boundary -
	 * 
	 *   This is the set of all points outside the region one of whose
	 *   HV Neighbors lies inside the region
	 * 
	 * Edge Boundary -
	 * 
	 *   This is the set of half points  that define the polygon which
	 *   forms the boundary  of the area represented  by the region if
	 *   we draw  each point as a square of side length 1  centered at
	 *   the given point. This is the most time consuming to calculate
	 *   and is ideally done only at the time of plotting.  Also, this
	 *   is the only boundary that is unambiguously 'traceable',  i.e.
	 *   given  the set of points  on the boundary,  we can decide the
	 *   unique ordering of the points which constitute a clockwise or
	 *   anticlockwise trace around the boundary
	 */
};

#endif