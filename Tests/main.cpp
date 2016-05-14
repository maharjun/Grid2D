#include <stdint.h>
#include <iostream>
#include <algorithm>

using namespace std;
#include "../Headers/Point.hpp"
#include "../Headers/Region.hpp"

int main() {

	Region ARegion(7, 7);

	/*
	 * This code inserts the following region
	 *
	 *     □ □ □ □ □ □ □   □ □ □ □ □ □ □   □ □ ■ ■ ■ □ □   □ □ ■ ■ ■ □ □
	 *     □ □ □ □ □ □ □   □ □ □ □ □ □ □   □ □ ■ ■ ■ □ □   □ □ ■ ■ ■ □ □
	 *     □ □ □ □ □ □ □   □ □ □ □ □ □ □   □ □ ■ ■ ■ □ □   □ □ ■ ■ ■ □ □
	 *     □ □ □ □ □ □ □ + □ □ □ □ □ □ □ + □ □ □ □ □ □ □ = □ □ □ □ □ □ □
	 *     ■ ■ ■ □ □ □ □   □ □ □ □ □ □ □   □ □ □ □ □ □ □   ■ ■ ■ □ □ □ □
	 *     ■ □ ■ □ □ □ □   □ ■ □ □ □ □ □   □ □ □ □ □ □ □   ■ ■ ■ □ □ □ □
	 *     ■ ■ ■ □ □ □ □   □ □ □ □ □ □ □   □ □ □ □ □ □ □   ■ ■ ■ □ □ □ □
	 *
	 * It firsts inserts the bottom points in random order and then inserts
	 * the point (1,1) after consolidating so that the insertion of (1,1)
	 * can test the 'inserting between two intervals'. This also tests the
	 * 'addition of a new interval' scenario in the case of the addition of
	 * (1,2)
	 *
	 * Then, we calculate the 3 boundaries and verify them.
	 *
	 * The 3 verified boundaries are as below:
	 *
	 * Inner Boundary:
	 *
	 *     □ □ ■ ■ ■ □ □
	 *     □ □ ■ □ ■ □ □
	 *     □ □ ■ ■ ■ □ □
	 *     □ □ □ □ □ □ □
	 *     ■ ■ ■ □ □ □ □
	 *     ■ □ ■ □ □ □ □
	 *     ■ ■ ■ □ □ □ □
	 *
	 * Outer Boundary:
	 *
	 *     □ ■ □ □ □ ■ □
	 *     □ ■ □ □ □ ■ □
	 *     □ ■ □ □ □ ■ □
	 *     ■ ■ ■ ■ ■ □ □
	 *     □ □ □ ■ □ □ □
	 *     □ □ □ ■ □ □ □
	 *     □ □ □ ■ □ □ □
	 *
	 * Total Midway Boundary (This is not tested because it is an intermediate variable:
	 *
	 *         ■ ■ ■ ■
	 *     □ □ ■ □ □ ■ □
	 *     □ □ ■ □ □ ■ □
	 *     □ □ ■ ■ ■ ■ □
	 *     ■ ■ ■ ■ □ □ □
	 *     ■ □ □ ■ □ □ □
	 *     ■ □ □ ■ □ □ □
	 *     ■ ■ ■ ■ □ □ □
	 *
	 * Returned Midway Boundary
	 *
	 *     □ □ □ □ □ □ □
	 *     □ □ □ □ □ □ □
	 *     □ □ □ □ □ □ □
	 *     ■ ■ ■ ■ □ □ □
	 *     ■ □ □ ■ □ □ □
	 *     ■ □ □ ■ □ □ □
	 *     ■ ■ ■ ■ □ □ □
	 */

	PointMexVect FirstSquare = {
		Point(0,0), Point(1,0), Point(2,0),
	    Point(0,1), Point(1,1), Point(2,1),
	    Point(0,2), Point(1,2), Point(2,2),
	};
	PointMexVect SecondSquare = {
		Point(2,4), Point(3,4), Point(4,4),
		Point(2,5), Point(3,5), Point(4,5),
		Point(2,6), Point(3,6), Point(4,6),
	};

	PointMexVect InnerBoundary = {
		Point(0,0), Point(1,0), Point(2,0),
		Point(0,1),             Point(2,1),
		Point(0,2), Point(1,2), Point(2,2),

		Point(2,4), Point(3,4), Point(4,4),
		Point(2,5),             Point(4,5),
		Point(2,6), Point(3,6), Point(4,6),
	};
	PointMexVect OuterBoundary = {
		                                    Point(3,0),
		                                    Point(3,1),
		                                    Point(3,2),
		Point(0,3), Point(1,3), Point(2,3), Point(3,3), Point(4,3),
		            Point(1,4),                                     Point(5,4),
		            Point(1,5),                                     Point(5,5),
		            Point(1,6),                                     Point(5,6),
	};
	PointMexVect MidwayBoundary = {
		Point(0, 0),
	    Point(0, 1),
	    Point(0, 2),
	    Point(0, 3),
	    Point(1, 3),
	    Point(2, 3),
	    Point(3, 3),
		Point(3, 2),
		Point(3, 1),
		Point(3, 0),
		Point(2, 0),
		Point(1, 0),
	}; // This is in clockwise rotation sequence

	// Insert all points in FirstSquare except (1,1)
	for (auto regionPoint : FirstSquare) {
		if (regionPoint != Point(1,1)) {
			ARegion.insert(regionPoint);
		}
	}

	// Consolidate, insert (1,1) and consolidate again.
	ARegion.consolidate();
	ARegion.insert(Point(1, 1));
	ARegion.consolidate();

	// Insert SecondSquare
	for (auto regionPoint : SecondSquare) {
		ARegion.insert(regionPoint);
	}

	// NOTE: There is no consolidation here.
	// this tests the call of implicit consolidation in the functions below.

	// Calculate all three boundaries
	auto ARegionInnerBoundary = ARegion.getInnerBoundary();
	auto ARegionOuterBoundary = ARegion.getOuterBoundary();
	auto ARegionMidwayBoundary = ARegion.getMidwayBoundary();

	assert(ARegionInnerBoundary == PointSet(InnerBoundary.begin(), InnerBoundary.end()));
	cout << "InnerBoundary successfully returned" << endl;

	assert(ARegionOuterBoundary == PointSet(OuterBoundary.begin(), OuterBoundary.end()));
	cout << "OuterBoundary successfully returned" << endl;

	assert(ARegionMidwayBoundary.size() == MidwayBoundary.size());
	assert(equal(MidwayBoundary.begin(), MidwayBoundary.end(), ARegionMidwayBoundary.begin()));
	cout << "MidwayBoundary successfully returned" << endl;

	cout << endl;
	cout << "Test was successful" << endl;
}