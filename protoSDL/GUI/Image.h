#pragma once
#include "Common.h"
#include <string>

namespace GUI
{
	class Image
	{
	public:
		virtual ~Image() = default;
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		Image(Image&&) = delete;
		Image& operator=(Image&&) = delete;

		static ImagePtr Create(SDL::RendererRef & renderer);

		operator bool() const { return IsSet(); }

		bool LoadFromFile(const char* fileName);
		void Draw(SDL_Point pos);
		void Draw(SDL_Rect pos);
		bool IsSet() const { return m_texture != nullptr; }

	protected:
		Image(SDL::RendererRef & renderer);

		SDL::RendererRef m_renderer;
		SDL::TexturePtr m_texture;

		int m_width;
		int m_height;

		struct shared_enabler;
	};
}