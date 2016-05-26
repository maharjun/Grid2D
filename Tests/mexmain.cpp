#include <algorithm>
#include <iostream>

#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include <MexMemoryInterfacing/Headers/MexMem.hpp>

#include "../Headers/Region.hpp"
#include "../MexHeaders/MexIO.hpp"

#include <mex.h>
#undef printf

using namespace std;

int main() {
	/*
	 * This is a test function. It takes no arguments and returns the boundary
	 * of the region described in the Test/main.cpp. Note that we DO NOT create
	 * the region and calculate its boundary in this function. That is assumed
	 * to be tested as a part of main.cpp. Here, we merely attempt to return
	 * the boundary vector and take it back as input.
	 */

	PointMexVect CorrectMidwayBoundary = {
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
		Point(0, 0),
		Point(2, 4),
		Point(2, 5),
		Point(2, 6),
		Point(2, 7),
		Point(3, 7),
		Point(4, 7),
		Point(5, 7),
		Point(5, 6),
		Point(5, 5),
		Point(5, 4),
		Point(4, 4),
		Point(3, 4),
		Point(2, 4),
		Point(0, 0)
	}; // This is in clockwise rotation sequence

	// Generate some incorrect boundaries
	MexVector<PointMexVect> IncorrectMidwayBoundaries(4, CorrectMidwayBoundary);
	IncorrectMidwayBoundaries[0].erase(3);
	IncorrectMidwayBoundaries[1].pop_back();
	IncorrectMidwayBoundaries[2].last() = Point(2, 1);
	IncorrectMidwayBoundaries[3].insert(7, {
		Point(4,3),
		Point(5,3),
		Point(6,3),
		Point(7,3),
		Point(8,3),
		Point(8,2),
		Point(7,2),
		Point(6,2),
		Point(5,2),
		Point(4,2)});

	auto Temp = CorrectMidwayBoundary;
	// This boundary is now converted into a mex array.
	// By the nature of assignmxArray
	mxArrayPtr MidwayBoundarymxArray = assignmxArray(Temp);

	// get CorrectMidwayBoundary Back.
	PointMexVect ProcessedBoundary;
	getInputfrommxArray(MidwayBoundarymxArray, ProcessedBoundary);

	assert(CorrectMidwayBoundary.size() == ProcessedBoundary.size());
	assert(std::equal(CorrectMidwayBoundary.begin(), CorrectMidwayBoundary.end(), ProcessedBoundary.begin()));
	cout << "Boundary successfully Processed in Input and Output (Without Type checking verifications)" << endl;

	Region regionFromBoundary(CorrectMidwayBoundary, 7, 7);
	PointMexVect RegionConvertedBoundary = regionFromBoundary.getMidwayBoundary();
	assert(std::equal(CorrectMidwayBoundary.begin(), CorrectMidwayBoundary.end(), ProcessedBoundary.begin()));
	cout << "Boundary successfully converted to and from region" << endl;

	// Try Processing Incorrect Boundaries
	for (size_t i = 0; i < IncorrectMidwayBoundaries.size(); ++i) {
		try {
			regionFromBoundary = Region(IncorrectMidwayBoundaries[i], 7, 7);
		}
		catch (RegionException E) {
			if (E == RegionException::INVALID_BOUNDARY) {
				cout << "Incorrect boundary " << i+1 << " successfully rejected" << endl;
			}
			else {
				throw(E);
			}
		}
	}

	cout << "Tests Successfully passed" << endl;
	return 0;
}