#include "Point.h"
#include "SDL_Rect.h"
#include <sstream>

namespace GUI
{
	std::string Point::ToString() const
	{
		std::ostringstream os;
		os << "P(" << this->x << "," << this->y << ")";
		return os.str();
	}
}