#pragma once
#include "Common.h"
#include <string>
#include <ostream>

namespace GUI
{
	class Point : public SDL_Point
	{
	public:
		Point() : SDL_Point({ 0, 0 }) {}
		Point(int x, int y) : SDL_Point({ x, y }) {};

		std::string ToString() const;
	};

	inline std::ostream & operator << (std::ostream & os, const Point& pt)
	{
		os << pt.ToString();
		return os;
	}
}