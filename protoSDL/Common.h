#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <memory>

namespace SDL
{
	using MainWindowRef = SDL_Window * ;
	using RendererRef = SDL_Renderer * ;
	using TextureRef = SDL_Texture * ;
	using FontRef = TTF_Font *;

	struct sdl_deleter
	{
		void operator()(MainWindowRef p) const { SDL_DestroyWindow(p); }
		void operator()(RendererRef p) const { SDL_DestroyRenderer(p); }
		void operator()(TextureRef p) const { SDL_DestroyTexture(p); }
		void operator()(FontRef p) const { TTF_CloseFont(p); }
	};

	using MainWindowPtr = std::unique_ptr<SDL_Window, sdl_deleter>;
	using RendererPtr = std::unique_ptr<SDL_Renderer, sdl_deleter>;
	using FontPtr = std::unique_ptr<TTF_Font, sdl_deleter>;
	using TexturePtr = std::shared_ptr<::SDL_Texture>;
}

namespace GUI
{
	class Image;
	using ImagePtr = std::shared_ptr<Image>;
	using ImageRef = Image * ;

	class Window;
	using WindowPtr = std::shared_ptr<Window>;
	using WindowRef = Window *;
}

