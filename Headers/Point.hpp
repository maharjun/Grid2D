#ifndef POINT_HPP
#define POINT_HPP

#include <stdint.h>
#include <MexMemoryInterfacing/Headers/MexMem.hpp>
#include <unordered_set>
#include <vector>

template <typename T = uint32_t>
struct GenericPoint {
	T x;
	T y;

	bool operator<(const GenericPoint &P) const {
		return (x < P.x || x == P.x && y < P.y);
	}

	GenericPoint() = default;
	GenericPoint(T x_, T y_) : x(x_), y(y_) {}

	GenericPoint(const GenericPoint& other) = default;

	template <typename T2>
	GenericPoint(const GenericPoint<T2>& other)
		: x(static_cast<T>(other.x)),
		  y(static_cast<T>(other.y)) {}

	GenericPoint& operator=(const GenericPoint& other) = default;

	template <typename T2>
	GenericPoint& operator=(const GenericPoint<T2>& other) {
		x = static_cast<T>(other.x);
		y = static_cast<T>(other.y);
		return *this;
	}

	bool operator==(const GenericPoint& rhs) const {
		return x == rhs.x
			&& y == rhs.y;
	}

	bool operator!=(const GenericPoint& rhs) const {
		return !this->operator==(rhs);
	}

	class hasher {
	public:
		size_t operator()(const GenericPoint& rhs) const {
			std::hash<T> THasher;
			return THasher(rhs.x) ^ THasher(rhs.y);
		}
	};
};

typedef GenericPoint<uint32_t> Point;
typedef std::unordered_set<Point, Point::hasher> PointSet;
typedef std::vector<Point> PointVect;
typedef MexVector<Point> PointMexVect;

typedef GenericPoint<float> SinglePoint;
typedef GenericPoint<double> DoublePoint;

#endif