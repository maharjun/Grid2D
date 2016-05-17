#ifndef ROW_COLUMN_REGION_HPP
#define ROW_COLUMN_REGION_HPP

#include "Point.hpp"
#include "LinearRegion.hpp"

class ColumnRegion : public LinearRegion {
private:
	uint32_t columnCoord;
	inline void ValidatePoint(const Point &P) const {
		// If debug, validate said point
		if (IS_DEBUG && P.x != this->columnCoord) {
			throw RegionException::COLUMN_COORD_NOT_MATCHED;
		}
	}
public:
	ColumnRegion() : columnCoord(UINT32_MAX) {}
	ColumnRegion(uint32_t regionCoord_, uint32_t regionLenLim_) : LinearRegion(regionLenLim_)
	{
		columnCoord = regionCoord_;
	}
	inline void insert(const Point &P) {
		ValidatePoint(P);
		LinearRegion::insert(P.y);
	}
	inline uint32_t getCoord() const
	{
		return columnCoord;
	}
	inline uint32_t find(const Point& P) const {
		ValidatePoint(P);
		return LinearRegion::find(P.y);
	}
};

struct RowRegion : LinearRegion {
private:
	uint32_t rowCoord;
	inline void ValidatePoint(const Point &P) const {
		// If debug, validate said point
		if (IS_DEBUG && P.y != this->rowCoord) {
			// ReSharper restore CppRedundantBooleanExpressionArgument
			throw RegionException::ROW_COORD_NOT_MATCHED;
		}
	}
public:
	RowRegion() : rowCoord(UINT32_MAX) {}
	RowRegion(uint32_t regionCoord_, uint32_t regionLenLim_) : LinearRegion(regionLenLim_)
	{
		rowCoord = regionCoord_;
	}
	inline void insert(const Point &P) {
		ValidatePoint(P);
		LinearRegion::insert(P.x);
	}
	inline uint32_t getCoord() const
	{
		return rowCoord;
	}
	inline uint32_t find(const Point &P) const {
		ValidatePoint(P);
		return LinearRegion::find(P.x);
	}
};

#endif