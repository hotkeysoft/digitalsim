#include <SDL.h>
#include "MenuItem.h"
#include "Label.h"
#include "RenderTarget.h"

namespace GUI
{
	MenuItem::MenuItem(RendererRef renderer, const char * id, const char * name) : Widget(id, renderer, nullptr, Rect(), name), m_opened(false)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}

		m_margin = 0;
		m_padding = 5;
		m_showBorder = false;
		m_borderWidth = 1;
	}

	MenuItemPtr MenuItem::Create(RendererRef renderer, const char * id, const char * name)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer, id, name);
		return std::static_pointer_cast<MenuItem>(ptr);
	}

	void MenuItem::Init()
	{
		std::cout << "MenuItem:: Init: " << GetId() << std::endl;
		m_label = Label::CreateAutoSize("l", m_renderer, Rect(), m_text.c_str(), nullptr, Label::TEXT_AUTOSIZE_DEFAULT, LCF_MENUITEM);
		m_label->SetPadding(Dimension(8, 0));
		m_label->Init();
	}

	void MenuItem::Draw(const PointRef rect)
	{
		if (!m_renderedMenu)
		{
			Render();
		}

		if (m_renderedMenu)
		{
			m_renderedMenuRect.x = rect->x;
			m_renderedMenuRect.y = rect->y;
			SDL_RenderCopy(m_renderer, m_renderedMenu.get(), nullptr, &m_renderedMenuRect);
		}
	}

	bool MenuItem::Hit(PointRef pt)
	{
		return m_labelRect.PointInRect(pt);
	}

	MenuItemPtr MenuItem::AddMenuItem(const char * id, const char * name)
	{
		m_renderedMenu = nullptr; // Force render next time menu is drawns

		if (id == nullptr)
		{
			throw std::invalid_argument("id is null");
		}

		MenuItemPtr item = MenuItem::Create(m_renderer, id, name);
		item->SetParent(this);
		item->Init();

		m_items.push_back(item);

		return item;
	}

	void MenuItem::Render()
	{
		std::cout << "Render: " << GetId() << std::endl;
		m_renderedMenuRect = Rect();

		m_renderedMenuRect.w = m_label->GetRect(true, false).w;
		for (auto item : m_items)
		{
			Rect labelRect = item->m_label->GetRect(true, false);
			m_renderedMenuRect.w = std::max(labelRect.w, m_renderedMenuRect.w);
			m_renderedMenuRect.h += labelRect.h + 2;
		}
		m_renderedMenuRect.h = std::max(m_renderedMenuRect.h, 10);
		m_renderedMenuRect.w += GetShrinkFactor().w * 2;
		m_renderedMenuRect.h += GetShrinkFactor().h * 2;

		TextureRef texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_renderedMenuRect.w, m_renderedMenuRect.h);
		if (texture)
		{
			RenderTarget target(m_renderer, texture);
			if (target)
			{
				DrawFilledRect(&m_renderedMenuRect, m_backgroundColor);
				Draw3dFrame(&m_renderedMenuRect, true);

				Rect target = Rect(m_renderedMenuRect.x + GetShrinkFactor().w, m_renderedMenuRect.y + GetShrinkFactor().h, 0, 0);

				for (auto item : m_items)
				{
					Rect labelRect = item->m_label->GetRect(true, false);
					target.w = labelRect.w;
					target.h = labelRect.h;

					item->m_label->Draw(&target, true);
					target.y += labelRect.h + 2;
				}

				m_renderedMenu = TexturePtr(texture, sdl_deleter());
			}
		}

	}

	struct MenuItem::shared_enabler : public MenuItem
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: MenuItem(std::forward<Args>(args)...)
		{
		}
	};
}