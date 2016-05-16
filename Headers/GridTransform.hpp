#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "Grid2D/Point.hpp"
#include <cmath>
#include <type_traits>

template<typename T,
         std::enable_if<
             std::is_same<T,double>::value &&
             std::is_same<T,float>::value>
             ::type>
struct GridTransform {
	/* This class contains the static methods that are responsible for trans-
	 * formations between the grid  coordinate system and the coordinate sys-
	 * tem of the state space of the relevant dynamical system.
	 *
	 * Functions contained:
	 *
	 *   1. toDynSysPoint(const DoublePoint &gridPoint)
	 *   2. toGridPoint(const DoublePoint &dynSysPoint)
	 *   3. test()
	 *
	 * It is the responsibility  of the creater of this  class to ensure that
	 * these two functions are actually the inverse of each other.
	 *
	 * The test()  function is a  function that returns a  boolean value that
	 * represents whether the given CoordTransform is valid or not. It basic-
	 * aly applies the composition  of the above two  functions and checks if
	 * the resultant is identity or not. It is executed on the vector of test
	 * points specified inside the function itself
	 *
	 */

    T scaleX, scaleY;
    T shiftX, shiftY;

	static inline DoublePoint toDynSysPoint(const DoublePoint &gridPoint) {
		return DoublePoint((gridPoint.x - shiftX)/scaleX, (gridPoint.y - shiftY)/scaleY);
	}
	static inline Point<T> toGridPoint(const Point<T> &dynSysPoint) {
		return Point<T>(scaleX*dynSysPoint.x + shiftX, scaleY*dynSysPoint.y + shiftY);
	}
	static inline bool test() {
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
			auto ReturnPoint = toGridPoint(toDynSysPoint(TestGridPoint));
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
