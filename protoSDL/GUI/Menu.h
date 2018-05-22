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

		static MenuPtr Create(RendererRef renderer, const char * id);

		MenuItemPtr AddMenuItem(const char * id, const char * name);

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;

		void Draw() override {};
		void Draw(const RectRef);

		void OpenMenu(MenuItemPtr item);
		void CloseMenu();

		int GetHeight() { return m_lineHeight + (2 * GetShrinkFactor().h); }

	protected:
		Menu(RendererRef renderer, const char * id);

		MenuItemPtr ItemAt(PointRef pt);

		int m_lineHeight;

		MenuItems m_items;

		struct shared_enabler;
	};
}