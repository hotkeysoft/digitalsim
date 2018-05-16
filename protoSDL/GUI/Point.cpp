#include "Point.h"
#include "SDL_Rect.h"
#include <sstream>

namespace GUI
{
	bool Point::IsEqual(const PointRef other) const
	{
		return (other != nullptr) &&
			(this->x == other->x) &&
			(this->y == other->y);
	}

	std::string Point::ToString() const
	{
		std::ostringstream os;
		os << "P(" << this->x << "," << this->y << ")";
		return os.str();
	}
}