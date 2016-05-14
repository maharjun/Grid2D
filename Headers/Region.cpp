#include <stdint.h>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include "Point.hpp"
#include "Region.hpp"

// ##################################################################
// PRIVATE FUNCTIONS
// ##################################################################

void Region::addNonAddedPoints() const {
	if (!isRowColUpdated) {
		std::vector<Point> nonAddedPointsVect(nonAddedPoints.begin(), nonAddedPoints.end());
		std::sort(nonAddedPointsVect.begin(), nonAddedPointsVect.end());
		for (auto &Point : nonAddedPointsVect) {
			gridColumns[Point.x].insert(Point);
			gridRows[Point.y].insert(Point);
		}
		nonAddedPoints.clear();
		isRowColUpdated = true;
	}
}

void Region::updateInnerBoundary() const {
	if (!isInnerBoundaryUpdated) {
		innerBoundary.clear();

		for (auto &rowRegion : gridRows) {
			auto &rowIntervals = rowRegion.getRegionIntervals();
			auto YCoord = rowRegion.getCoord();
			for (auto rowInterval : rowIntervals) {
				innerBoundary.insert(Point(rowInterval.beginPoint, YCoord));
				innerBoundary.insert(Point(rowInterval.endPoint - 1, YCoord));
			}
		}

		for (auto &colRegion : gridColumns) {
			auto &colIntervals = colRegion.getRegionIntervals();
			auto XCoord = colRegion.getCoord();
			for (auto colInterval : colIntervals) {
				innerBoundary.insert(Point(XCoord, colInterval.beginPoint));
				innerBoundary.insert(Point(XCoord, colInterval.endPoint - 1));
			}
		}
		isInnerBoundaryUpdated = true;
	}
}

// ##################################################################
// PUBLIC FUNCTIONS
// ##################################################################

void Region::consolidate() const {
	/* This function forcefully performs all the update steps.  Currently the
	* following:
	*
	* 1.  updateInnerBoundary()
	* 2.  addNonAddedPoints()
	*
	* After these  steps all the data members will be fully represenative of
	* the region they represent
	*/
	addNonAddedPoints();
	updateInnerBoundary();
}

PointSet Region::getOuterBoundary() const
{
	consolidate();
	PointSet outerBoundary;
	// Getting the outer boundary points from all the grid columns
	for (auto &Column : gridColumns) {
		auto &columnIntervals = Column.getRegionIntervals();
		for (auto columnInterval : columnIntervals) {
			if(columnInterval.beginPoint > 0)
				outerBoundary.insert(Point(Column.getCoord(), columnInterval.beginPoint - 1));
			if(columnInterval.endPoint < gridYLim)
				outerBoundary.insert(Point(Column.getCoord(), columnInterval.endPoint));
		}
	}
	// Getting the outer boundary points from all the grid rows
	for(auto &Row: gridRows) {
		auto &rowIntervals = Row.getRegionIntervals();
		for (auto rowInterval : rowIntervals) {
			if (rowInterval.beginPoint > 0)
				outerBoundary.insert(Point(rowInterval.beginPoint - 1, Row.getCoord()));
			if (rowInterval.endPoint < gridXLim)
				outerBoundary.insert(Point(rowInterval.endPoint, Row.getCoord()));
		}
	}
	return outerBoundary;
}

PointMexVect Region::getMidwayBoundary() const {
	/* 
	 * This function returns the Midway boundary corresponding to the region.
	 * The midway boundary  consists of half points that  actually define the
	 * boundary of the given region.  This vector of points can be plotted as
	 * a polygon in order to plot the region on a graph. The vector gives the
	 * boundary in clockwise direction  (i.e. such that the winding number of
	 * the curve is negative w.r.t a point in the interior)
	 * 
	 * create a EdgeMap which is an unordered map as follows (Python Dict
	 * syntax)
	 * 
	 * {BoundaryPoint:NextBoundaryPoint}

	 * for each C in gridColumns:
	 *     for each contained interval:
	 *         Add upper edge of upper boundary in right direction to EdgeMap
	 *         Add lower edge of lower boundary in left direction to EdgeMap
	 * for each R in gridRows:
	 *     for each contained interval:
	 *         Add left edge of left boundary in upward direction to EdgeMap
	 *         Add right edge of right boundary in downward direction to EdgeMap
	 * 
	 * Find Least Point.
	 * 
	 * Trace boundary using The linked list represented by EdgeMap with the
	 * starting as the least point with the starting and return vector
	 * 
	 */
	
	consolidate();

	typedef std::pair<Point, Point> Edge;
	typedef std::unordered_map<Point, Point, Point::hasher> EdgeMap;

	EdgeMap boundaryEdges;
	for(auto &column : gridColumns) {
		auto &colIntervals = column.getRegionIntervals();
		for(auto interval:colIntervals) {
			// Bottom Edge inserted in left direction
			boundaryEdges.insert(Edge(
				Point(column.getCoord() + 1, interval.beginPoint),
				Point(column.getCoord(), interval.beginPoint)
			));
			// Top Edge inserted in right direction
			boundaryEdges.insert(Edge(
				Point(column.getCoord(), interval.endPoint),
				Point(column.getCoord() + 1, interval.endPoint)
			));
		}
	}

	uint32_t firstRow = 0;
	bool foundFirstRow = false;
	for (auto &row : gridRows) {
		auto &rowIntervals = row.getRegionIntervals();
		if(!rowIntervals.isempty() && !foundFirstRow) {
			// This is to calculate the first non-empty row
			firstRow = row.getCoord();
			foundFirstRow = true;
		}
		for (auto interval : rowIntervals) {
			// Left Edge inserted in upward direction.
			boundaryEdges.insert(Edge(
				Point(interval.beginPoint, row.getCoord()),
				Point(interval.beginPoint, row.getCoord() + 1)
			));
			// Right Edge inserted in downward direction.
			boundaryEdges.insert(Edge(
				Point(interval.endPoint, row.getCoord() + 1),
				Point(interval.endPoint, row.getCoord())
			));
		}
	}

	if (foundFirstRow) {
		// This means that there is at-least one non-empty row
		// Hence we assign the min-point (min by lexicographical ordering)
		Point minPoint(
			gridRows[firstRow].getRegionIntervals()[0].beginPoint,
			gridRows[firstRow].getCoord());
		
		PointMexVect boundaryVect;
		auto currentPoint = minPoint;
		do {
			boundaryVect.push_back(currentPoint);
			auto nextPoint = boundaryEdges[currentPoint];
			boundaryEdges.erase(currentPoint);
			currentPoint = nextPoint;
		} while (currentPoint != minPoint);
		
		return boundaryVect;
	}
	else {
		// This means the region is empty. So return empty border
		return PointMexVect();
	}
}
