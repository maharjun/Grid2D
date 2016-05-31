#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Point.hpp"
#include "RegionExc.hpp"
#include <MexMemoryInterfacing/Headers/GenericMexIO.hpp>
#include <cmath>
#include <type_traits>

template<typename T,
         typename B = typename std::enable_if<std::is_floating_point<T>::value>::type>
struct GridTransform {

    T scaleX, scaleY;
    T shiftX, shiftY;

	inline GenericPoint<T> toActualCoords(const GenericPoint<T> &gridPoint) const {
		return GenericPoint<T>(scaleX*gridPoint.x + shiftX, scaleY*gridPoint.y + shiftY);
	}
	inline GenericPoint<T> toGridCoords(const GenericPoint<T> &actualPoint) const {
		return GenericPoint<T>((actualPoint.x - shiftX)/scaleX, (actualPoint.y - shiftY)/scaleY);
	}
	inline Point toGridPoint(const GenericPoint<T> &actualPoint) {
		// Converts the ActualPoint to Grid Coordinates and rounds off to the
		// nearest integer and returns
		// NOTE: This function will fail disastrously if any of the coordinates
		// of actualPoint are negative. Hence in Debug Mode, a check is introduced for the same
		auto pointInGridCoords = toGridCoords(actualPoint);
		if (IS_DEBUG && pointInGridCoords.x < T(-0.5) && pointInGridCoords.y < T(-0.5)) {
			WriteException(
				RegionException::NEGATIVE_GRID_POINT,
				"The Grid Point (%f, %f), which corresponds to the Actual Point (%f, %f), \n"
					"has one of its components closest to a negative integer and thus cannot be \n"
					"rounded off to an unsigned integer representing the closest grid point",
			    pointInGridCoords.x, pointInGridCoords.y,
			    actualPoint.x, actualPoint.y
			);
		}
		else {
			return Point(pointInGridCoords.x+T(0.5), pointInGridCoords.y+T(0.5));
		}
	}
	inline bool test() const {
		std::vector<DoublePoint> TestGridPointVect = {
			GenericPoint<T>(0,0),
			GenericPoint<T>(1,0),
			GenericPoint<T>(0,1),
			GenericPoint<T>(2,3),
			GenericPoint<T>(1.63,2.42)
		};
		bool isSuccessful = true;
		double eps = 1e-10;
		for(auto &TestGridPoint : TestGridPointVect) {
			auto ReturnPoint = toGridCoords(toActualCoords(TestGridPoint));
			if (
				std::abs(ReturnPoint.x - TestGridPoint.x) >= eps ||
				std::abs(ReturnPoint.y - TestGridPoint.y) >= eps) {
				isSuccessful = false;
				break;
			}
		}

		return isSuccessful;
	}
};

#endif
