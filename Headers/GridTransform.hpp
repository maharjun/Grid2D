#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Grid2D/Headers/Point.hpp"
#include <cmath>
#include <type_traits>

template<typename T,
         std::enable_if<
             std::is_same<T,double>::value &&
             std::is_same<T,float>::value>
             ::type>
struct GridTransform {

    T scaleX, scaleY;
    T shiftX, shiftY;

	inline DoublePoint toActualPoint(const DoublePoint &gridPoint) const {
		return Point<T>(scaleX*gridPoint.x + shiftX, scaleY*gridPoint.y + shiftY);
	}
	inline Point<T> toGridPoint(const Point<T> &actualPoint) const {
		return DoublePoint((actualPoint.x - shiftX)/scaleX, (actualPoint.y - shiftY)/scaleY);
	}
	inline bool test() const {
		std::vector<DoublePoint> TestGridPointVect = {
			DoublePoint(0,0),
			DoublePoint(1,0),
			DoublePoint(0,1),
			DoublePoint(2,3),
			DoublePoint(1.63,2.42)
		};
		bool isSuccessful = true;
		double eps = 1e-10;
		for(auto &TestGridPoint : TestGridPointVect) {
			auto ReturnPoint = toGridPoint(toActualPoint(TestGridPoint));
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
