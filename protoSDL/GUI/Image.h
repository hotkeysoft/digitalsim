#pragma once
#include "Common.h"
#include "Rect.h"
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

		static ImagePtr Create(RendererRef & renderer);

		operator bool() const { return IsSet(); }

		bool LoadFromFile(const char* fileName);
		void Draw(SDL_Point pos);
		void Draw(Rect pos);
		bool IsSet() const { return m_texture != nullptr; }

	protected:
		Image(RendererRef & renderer);

		RendererRef m_renderer;
		TexturePtr m_texture;

		int m_width;
		int m_height;

		struct shared_enabler;
	};
}