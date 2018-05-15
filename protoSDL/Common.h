#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <memory>
#include <iostream>

namespace GUI
{
	enum HitZone : uint16_t {
		HIT_NOTHING = 0,
		HIT_TITLEBAR = 1,
		HIT_CLIENT = 2,

		HIT_BORDER_TOP = 4,
		HIT_BORDER_BOTTOM = 8,
		HIT_BORDER_LEFT = 16,
		HIT_BORDER_RIGHT = 32,

		HIT_CORNER_TOPLEFT = 64,
		HIT_CORNER_TOPRIGHT = 128,
		HIT_CORNER_BOTTOMLEFT = 256,
		HIT_CORNER_BOTTOMRIGHT = 512,

		HIT_BORDER_ANY = HIT_BORDER_LEFT | HIT_BORDER_RIGHT | HIT_BORDER_TOP | HIT_BORDER_BOTTOM,
		HIT_CORNER_ANY = HIT_CORNER_TOPLEFT | HIT_CORNER_TOPRIGHT | HIT_CORNER_BOTTOMLEFT | HIT_CORNER_BOTTOMRIGHT,

		HIT_SYSMENU = 1024,
		HIT_MINBUTTON = 2048,
		HIT_MAXBUTTON = 4096,

		HIT_BUTTON_ANY = HIT_SYSMENU | HIT_MINBUTTON | HIT_MAXBUTTON
	};

	// Creation flags.  These attribute don't change during the lifetime of the window
	enum class WindowFlags {
		WIN_SYSMENU = 1, // Top left button in titlebar (left side)
		WIN_MINMAX = 2, // Minimize / Maximize buttons in titlebar (right side)
		WIN_CANMOVE = 4, // Window can be moved
		WIN_CANRESIZE = 8, // Window can me resized
		WIN_ACTIVE = 16, // Always look active, usually for main window

		WIN_DEFAULT = WIN_SYSMENU | WIN_MINMAX | WIN_CANMOVE | WIN_CANRESIZE
	};

	enum WindowState : uint16_t {
		WS_VISIBLE = 1,
		WS_MAXIMIZED = 2,
		WS_MINIMIZED = 4,
	};

	constexpr enum WindowFlags operator |(const enum WindowFlags selfValue, const enum WindowFlags inValue)
	{
		return (enum WindowFlags)(uint32_t(selfValue) | uint32_t(inValue));
	}

	constexpr bool operator &(const enum WindowFlags selfValue, const enum WindowFlags inValue)
	{
		return (uint32_t(selfValue) & uint32_t(inValue));
	}

	class Image;
	using ImagePtr = std::shared_ptr<Image>;
	using ImageRef = Image * ;

	class Window;
	using WindowPtr = std::shared_ptr<Window>;
	using WindowRef = Window * ;

	// SDL Wrappers
	using MainWindowRef = SDL_Window * ;
	using RendererRef = SDL_Renderer * ;
	using TextureRef = SDL_Texture * ;
	using FontRef = TTF_Font *;
	using CursorRef = SDL_Cursor * ;

	struct sdl_deleter
	{
		void operator()(MainWindowRef p) const { SDL_DestroyWindow(p); std::cout << "DestroyWindow" << std::endl; }
		void operator()(RendererRef p) const { SDL_DestroyRenderer(p); std::cout << "DestroyRenderer" << std::endl; }
		void operator()(TextureRef p) const { SDL_DestroyTexture(p); std::cout << "DestroyTexture" << std::endl; }
		void operator()(FontRef p) const { TTF_CloseFont(p); std::cout << "CloseFont" << std::endl; }
		void operator()(CursorRef p) const { SDL_FreeCursor(p); std::cout << "FreeCursor" << std::endl; }
	};

	using MainWindowPtr = std::unique_ptr<SDL_Window, sdl_deleter>;
	using RendererPtr = std::unique_ptr<SDL_Renderer, sdl_deleter>;
	using FontPtr = std::unique_ptr<TTF_Font, sdl_deleter>;
	using CursorPtr = std::unique_ptr<SDL_Cursor, sdl_deleter>;
	using TexturePtr = std::shared_ptr<::SDL_Texture>;

	template <typename T>
	T clip(const T& n, const T& lower, const T& upper) {
		return std::max(lower, std::min(n, upper));
	}
}

