#include <SDL.h>
#include "ClipRect.h"
#include "Menu.h"

namespace GUI
{
	Menu::Menu(RendererRef renderer) : Widget("Menu", renderer, nullptr, Rect(), nullptr)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}

		m_padding = 5;
		m_margin = 0;
	}

	MenuPtr Menu::Create(RendererRef renderer)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer);
		return std::static_pointer_cast<Menu>(ptr);
	}

	void Menu::Init()
	{
		m_lineHeight = TTF_FontLineSkip(m_font);
	}

	void Menu::Draw(const RectRef rect)
	{
		Rect drawRect = *rect;
		Rect frameRect;

		drawRect.h = GetHeight();

		Draw3dFrame(&drawRect, true);

		drawRect = drawRect.Deflate(GetShrinkFactor());

		ClipRect clip(m_renderer, &frameRect);
		if (clip)
		{
//			DrawTree(&drawRect);
		}
	}

	MenuItemPtr Menu::AddMenuItem(const char * name)
	{
		return MenuItemPtr();
	}

	struct Menu::shared_enabler : public Menu
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Menu(std::forward<Args>(args)...)
		{
		}
	};
}