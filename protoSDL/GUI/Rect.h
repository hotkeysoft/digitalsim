#pragma once
#include "Common.h"
#include "Point.h"
#include <string>
#include <ostream>

namespace GUI
{
	class Rect : public SDL_Rect
	{
	public:
		Rect() : SDL_Rect({ 0, 0, 0, 0 }) {}
		Rect(int x, int y, int w, int h) : SDL_Rect({ x, y, w, h }) {};

		bool IsEmpty() const;
		bool IsEqual(const RectRef other) const;

		bool PointInRect(const PointRef pt) { return SDL_PointInRect(pt, this); }
		Rect IntersectRect(const RectRef);

		Point Origin() const { return Point(x, y); }
		Point Size() const { return Point(w, h); }

		Rect Offset(const PointRef pt) const { return Rect(x + pt->x, y + pt->y, w, h); }
		Rect OffsetNeg(const PointRef pt) const { return Rect(x - pt->x, y - pt->y, w, h); }

		static bool PointInRect(const PointRef, const RectRef);
		static bool IntersectRect(const RectRef r1, const RectRef r2, const RectRef out);

		std::string ToString() const;
	};

	inline std::ostream & operator << (std::ostream & os, const Rect& rect)
	{
		os << rect.ToString();
		return os;
	}
}