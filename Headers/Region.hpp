#ifndef REGION_HPP
#define REGION_HPP

#include <stdint.h>

#include <MexMemoryInterfacing/Headers/MexMem.hpp>
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

	inline const PointSet & getInnerBoundary() const {
		consolidate();
		return this->innerBoundary;
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
			isContained = gridColumns[point2Search.x].find(point2Search);
		}
		return isContained;
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