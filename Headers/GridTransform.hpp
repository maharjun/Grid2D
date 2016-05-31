#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Point.hpp"
#include <cmath>
#include <type_traits>

template<typename T,
         typename B = typename std::enable_if<std::is_floating_point<T>::value>::type>
struct GridTransform {

    T scaleX, scaleY;
    T shiftX, shiftY;

	inline GenericPoint<T> toActualPoint(const GenericPoint<T> &gridPoint) const {
		return GenericPoint<T>(scaleX*gridPoint.x + shiftX, scaleY*gridPoint.y + shiftY);
	}
	inline GenericPoint<T> toGridPoint(const GenericPoint<T> &actualPoint) const {
		return GenericPoint<T>((actualPoint.x - shiftX)/scaleX, (actualPoint.y - shiftY)/scaleY);
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
