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
		DECLARE_EVENT_CLASS_NAME(Menu)

		enum TextBoxEvents : EventCode
		{
			EVENT_MENU_SELECTED // MenuItemRef in data2
		};

		using MenuItems = std::vector<MenuItemPtr>;
		using HotkeyMap = std::map<SDL_Keycode, MenuItemPtr>;

		virtual ~Menu() = default;
		Menu(const Menu&) = delete;
		Menu& operator=(const Menu&) = delete;
		Menu(Menu&&) = delete;
		Menu& operator=(Menu&&) = delete;

		void Init() override;

		static MenuPtr Create(RendererRef renderer, const char * id);

		MenuItemPtr AddMenuItem(const char * id, const char * name, SDL_Keycode hotkey = SDLK_UNKNOWN);

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;

		void Draw() override {};
		void Draw(const RectRef);

		void OpenMenu(MenuItemPtr item);
		void CloseMenu();

		int GetHeight() { return m_lineHeight + (2 * GetShrinkFactor().h); }

		void MoveLeft();
		void MoveRight();
		void MoveUp();
		void MoveDown();

	protected:
		Menu(RendererRef renderer, const char * id);

		MenuItemPtr ItemAt(PointRef pt);

		void DrawActiveFrame(MenuItemRef parent);
		void CloseMenuItem(MenuItemPtr item);

		MenuItems::const_iterator FindMenuItem(MenuItemRef item) const;

		MenuItemPtr m_active;
		int m_lineHeight;
		MenuItems m_items;
		HotkeyMap m_hotkeys;

		struct shared_enabler;
	};
}