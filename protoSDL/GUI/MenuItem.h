#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Point.h"
#include <vector>

namespace GUI
{
	class MenuItem : public Widget
	{
	public:
		using MenuItems = std::vector<MenuItemPtr>;

		virtual ~MenuItem() = default;
		MenuItem(const MenuItem&) = delete;
		MenuItem& operator=(const MenuItem&) = delete;
		MenuItem(MenuItem&&) = delete;
		MenuItem& operator=(MenuItem&&) = delete;

		void Init() override;

		static MenuItemPtr Create(RendererRef renderer, const char * id, const char * name);

		MenuItemPtr AddMenuItem(const char * id, const char * name);

		bool Hit(PointRef pt);

		void Draw() override {}
		void Draw(const PointRef pt);

		bool IsOpened() { return m_opened; }
		LabelRef GetLabel() { return m_label.get(); }

	protected:
		MenuItem(RendererRef renderer, const char * id, const char * name);

		void Render();
		Rect DrawFrame(const RectRef &rect);

		bool m_opened;
		LabelPtr m_label;
		Rect m_labelRect;
		MenuItems m_items;
		TexturePtr m_renderedMenu;
		Rect m_renderedMenuRect;

		struct shared_enabler;

		friend class Menu;
	};
}