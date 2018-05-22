#include <SDL.h>
#include "MenuItem.h"
#include "Label.h"
#include "RenderTarget.h"

namespace GUI
{
	MenuItem::MenuItem(RendererRef renderer, const char * id, const char * name, MenuItemRef parent) : 
		Widget(id, renderer, parent, Rect(), name), m_opened(false)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}

		m_margin = 0;
		m_padding = 4;
		m_showBorder = false;
		m_borderWidth = 1;
	}

	MenuItemPtr MenuItem::Create(RendererRef renderer, const char * id, const char * name, MenuItemRef parent)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer, id, name, parent);
		return std::static_pointer_cast<MenuItem>(ptr);
	}

	void MenuItem::Init()
	{
		m_label = Label::CreateAutoSize("l", m_renderer, Rect(), m_text.c_str(), nullptr, Label::TEXT_AUTOSIZE_DEFAULT, LCF_MENUITEM);

		m_label->SetPadding(Dimension(8, 2));
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
		if (m_labelRect.PointInRect(pt))
			return true;

		if (IsOpened())
		{
			return m_renderedMenuRect.PointInRect(pt);
		}

		return false;
	}

	MenuItemPtr MenuItem::ItemAt(PointRef pt)
	{
		if (m_labelRect.PointInRect(pt))
		{
			return shared_from_this();
		}

		if (IsOpened())
		{
			Point rel(pt->x - m_renderedMenuRect.x, pt->y - m_renderedMenuRect.y);

			for (auto & item : m_items)
			{			
				Rect & rect = item->m_rect;

				if (rect.PointInRect(&rel))
				{
					return item;
				}
			}
		}

		return nullptr;
	}

	MenuItemPtr MenuItem::AddMenuItem(const char * id, const char * name)
	{
		m_renderedMenu = nullptr; // Force render next time menu is drawns

		if (id == nullptr)
		{
			throw std::invalid_argument("id is null");
		}

		MenuItemPtr item = MenuItem::Create(m_renderer, id, name, this);
		item->Init();

		m_items.push_back(item);

		return item;
	}

	void MenuItem::Render()
	{
		m_renderedMenuRect = Rect();

		m_renderedMenuRect.w = m_label->GetRect(true, false).w;
		for (auto & item : m_items)
		{
			Rect labelRect = item->m_label->GetRect(true, false);
			m_renderedMenuRect.w = std::max(labelRect.w, m_renderedMenuRect.w);
			m_renderedMenuRect.h += labelRect.h;// + 2;
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

				Rect target = m_renderedMenuRect.Deflate(GetShrinkFactor());

				for (auto & item : m_items)
				{				
					Rect labelRect = item->m_label->GetRect(true, false);
					target.h = labelRect.h;

					item->m_label->Draw(&target, true);
					item->m_rect = target;

					target.y += labelRect.h;// + 2;
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