#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Point.h"
#include <string>
#include <vector>

namespace GUI
{
	class Menu : public Widget
	{
	public:
		using MenuItems = std::vector<MenuItemPtr>;

		virtual ~Menu() = default;
		Menu(const Menu&) = delete;
		Menu& operator=(const Menu&) = delete;
		Menu(Menu&&) = delete;
		Menu& operator=(Menu&&) = delete;

		void Init() override;

		static MenuPtr Create(RendererRef renderer);

		MenuItemPtr AddMenuItem(const char * name);

		void Draw() override {};
		void Draw(const RectRef);

		int GetHeight() { return m_lineHeight + (2 * GetShrinkFactor().h); }

	protected:
		Menu(RendererRef renderer);

		int m_lineHeight;

		MenuItems m_items;

		struct shared_enabler;
	};
}