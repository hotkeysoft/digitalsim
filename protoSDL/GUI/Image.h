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
		virtual ~Image() = default;
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;
		Image(Image&&) = delete;
		Image& operator=(Image&&) = delete;

		static ImagePtr Create(RendererRef renderer);

		operator bool() const { return IsSet(); }

		bool LoadFromFile(const char* fileName);
		void Draw() override {}
		void Draw(const PointRef pos);
		void Draw(const RectRef pos);
		bool IsSet() const { return m_texture != nullptr; }

	protected:
		Image(RendererRef renderer);

		TexturePtr m_texture;

		int m_width;
		int m_height;

		struct shared_enabler;
	};
}