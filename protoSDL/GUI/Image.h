#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Point.h"
#include <string>

namespace GUI
{
	class Image : public Widget
	{
	public:
		enum ImageAlign : uint8_t {
			IMG_H_LEFT = 1,
			IMG_H_RIGHT = 2,
			IMG_H_CENTER = IMG_H_LEFT | IMG_H_RIGHT,

			IMG_V_TOP = 8,
			IMG_V_BOTTOM = 16,
			IMG_V_CENTER = IMG_V_TOP | IMG_V_BOTTOM,

			IMG_CENTER = IMG_H_CENTER | IMG_V_CENTER,

			IMG_DEFAULT = IMG_H_LEFT | IMG_V_TOP,
		};

		virtual ~Image() = default;
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		Image(Image&&) = delete;
		Image& operator=(Image&&) = delete;

		static ImagePtr FromFile(RendererRef renderer, const char* fileName);
		static ImagePtr FromTexture(RendererRef renderer, TexturePtr texture);

		explicit operator bool() const { return IsSet(); }

		void Draw() override {}
		void Draw(const PointRef pos);
		void Draw(const RectRef rect, uint8_t align = IMG_DEFAULT);
		bool IsSet() const { return m_texture != nullptr; }

	protected:
		Image(RendererRef renderer);

		bool LoadFromFile(const char* fileName);
		bool LoadFromTexture(TexturePtr);

		TexturePtr m_texture;

		struct shared_enabler;
	};
}