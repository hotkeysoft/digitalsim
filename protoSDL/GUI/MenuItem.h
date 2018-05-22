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

		static MenuItemPtr Create(RendererRef renderer);

		MenuItemPtr AddMenuItem(const char * name);

		void Draw() override {}
		void Draw(const RectRef rect);

	protected:
		MenuItem(RendererRef renderer);

		MenuItems m_items;

		struct shared_enabler;
	};
}