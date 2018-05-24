#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Point.h"
#include <vector>

namespace GUI
{
	class MenuItem : public Widget, public std::enable_shared_from_this<MenuItem>
	{
	public:
		using MenuItems = std::vector<MenuItemPtr>;
		using HotkeyMap = std::map<SDL_Keycode, MenuItemPtr>;

		virtual ~MenuItem() = default;
		MenuItem(const MenuItem&) = delete;
		MenuItem& operator=(const MenuItem&) = delete;
		MenuItem(MenuItem&&) = delete;
		MenuItem& operator=(MenuItem&&) = delete;

		void Init() override;

		static MenuItemPtr Create(RendererRef renderer, const char * id, const char * name, MenuItemRef parent);

		MenuItemPtr AddMenuItem(const char * id, const char * name, SDL_Keycode hotkey = SDLK_UNKNOWN);
		MenuItemRef GetParentMenuItem() { return dynamic_cast<MenuItemRef>(m_parent); }

		bool Hit(PointRef pt);

		void Draw() override {}
		void Draw(const PointRef pt);

		bool IsOpened() { return m_opened; }
		bool HasSubMenu() { return !m_items.empty(); }
		LabelRef GetLabel() { return m_label.get(); }

	protected:
		MenuItem(RendererRef renderer, const char * id, const char * name, MenuItemRef parent);

		MenuItemPtr ItemAt(PointRef pt);

		void Render();

		bool m_opened;
		LabelPtr m_label;
		Rect m_labelRect;
		MenuItems m_items;
		TexturePtr m_renderedMenu;
		Rect m_renderedMenuRect;

		struct shared_enabler;

		HotkeyMap m_hotkeys;

		friend class Menu;
	};
}