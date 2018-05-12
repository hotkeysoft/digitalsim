#pragma once
#include "Common.h"
#include <string>

namespace GUI
{
	class Color : public SDL_Color
	{
	public:
		Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) : SDL_Color({ red, green, blue, alpha }) {};

		static const Color C_WHITE;
		static const Color C_BLACK;
		static const Color C_LIGHT_GREY;
		static const Color C_DARK_GREY;
	};
}