#include "Rect.h"
#include "SDL_Rect.h"
#include <sstream>

namespace GUI
{
	Rect Rect::IntersectRect(const RectRef otherRect)
	{
		Rect ret;
		SDL_IntersectRect(this, otherRect, &ret);
		return ret;
	}
	bool Rect::PointInRect(const SDL_Point* pt, const RectRef rect)
	{
		return SDL_PointInRect(pt, rect);
	}

	bool Rect::IntersectRect(const RectRef r1, const RectRef r2, const RectRef out)
	{
		return SDL_IntersectRect(r1, r2, out);
	}

	std::string Rect::ToString() const
	{
		std::ostringstream os;
		os << "RECT(xy[" << this->x << "," << this->y << "],wh[" << this->w << "," << this->h << "])";
		return os.str();
	}
}