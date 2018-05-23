#include <SDL.h>
#include "ClipRect.h"
#include "Menu.h"
#include "Label.h"
#include "MenuItem.h"

namespace GUI
{
	Menu::Menu(RendererRef renderer, const char * id) : Widget(id, renderer, nullptr, Rect(), nullptr), m_lineHeight(0)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}

		m_padding = Dimension(2, 2);
		m_margin = 0;
	}

	MenuPtr Menu::Create(RendererRef renderer, const char * id)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer, id);
		return std::static_pointer_cast<Menu>(ptr);
	}

	void Menu::Init()
	{
		if (m_lineHeight == 0)
		{
			// Fallback
			m_lineHeight = TTF_FontLineSkip(m_font);
		}
	}

	void Menu::Draw(const RectRef rect)
	{
		Rect drawRect = *rect;
		drawRect.h = GetHeight();

		Draw3dFrame(&drawRect, true);

		drawRect = drawRect.Deflate(GetShrinkFactor());
		m_rect = drawRect;

		//TODO: Get main window size
		ClipRect clip(m_renderer, &Rect(0,0,2000,2000), false);
		if (clip)
		{
			for (auto & item : m_items)
			{
				Rect labelRect = item->GetLabel()->GetRect(true, false);				
				drawRect.w = labelRect.w;

				if (item->IsOpened())
				{
					Draw3dFrame(&drawRect, false);
					Point menuPos(drawRect.Origin());
					menuPos.y += (labelRect.h);

					// Menu is on top of everything so no need for bounding rect
					item->Draw(&menuPos);
				}

				if (m_active)
				{
					MenuItemRef parent = m_active->GetParentMenuItem();
					if (parent)
					{
						Rect highlight = m_active->m_rect.Offset(&parent->m_renderedMenuRect.Origin());
						Draw3dFrame(&highlight, false);
					}
				}

				item->m_label->Draw(&drawRect);
				item->m_labelRect = drawRect;

				drawRect.x += labelRect.w;
			}
		}
	}

	MenuItemPtr Menu::AddMenuItem(const char * id, const char * name)
	{
		if (id == nullptr)
		{
			throw std::invalid_argument("id is null");
		}

		MenuItemPtr item = MenuItem::Create(m_renderer, id, name, nullptr);
		item->SetParent(this);
		item->Init();
		m_lineHeight = item->m_label->GetRect(true, false).h;

		m_items.push_back(item);

		return item;
	}

	HitResult Menu::HitTest(const PointRef pt)
	{		
		if (m_rect.PointInRect(pt))
		{
			return HitResult(HitZone::HIT_MENU, this);
		}

		for (auto & item : m_items)
		{
			if (item->IsOpened() && item->m_renderedMenuRect.PointInRect(pt))
			{
				return HitResult(HitZone::HIT_MENU_ITEM, item.get());
			}
		}

		return HitZone::HIT_NOTHING;
	}

	MenuItemPtr Menu::ItemAt(PointRef pt)
	{
		auto it = std::find_if(m_items.begin(), m_items.end(), [pt](MenuItemPtr item) { return item->Hit(pt); });
		if (it != m_items.end())
		{
			return (*it)->ItemAt(pt);
		}

		return nullptr;
	}

	bool Menu::HandleEvent(SDL_Event * e)
	{
		Point pt(e->button.x, e->button.y);
		HitResult hit = HitTest(&pt);
		const CaptureInfo & capture = WINMGR().GetCapture();
		switch (e->type)
		{
		case SDL_MOUSEMOTION:
			if (hit)
			{
				SDL_SetCursor(RES().FindCursor("default"));
				if (capture)
				{
					MenuItemPtr item = ItemAt(&pt);
					if (item && hit == HIT_MENU)
					{
						m_active = nullptr;
						OpenMenu(item);
					}
					else if (item && hit == HIT_MENU_ITEM)
					{
						m_active = item;
					}
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			MenuItemPtr item = ItemAt(&pt);

			if (capture)
			{
				MenuItemPtr item = ItemAt(&pt);
				CloseMenu();
				WINMGR().ReleaseCapture();
				if (item && hit == HIT_MENU_ITEM)
				{
					PostEvent(EVENT_MENU_SELECTED, item.get());
				}
				return true;
			}

			SetActive();
			SetFocus(this);

			if (item)
			{
				OpenMenu(item);
				WINMGR().StartCapture(hit, &pt);
			}

			return item != nullptr;
		}
		case SDL_MOUSEBUTTONUP:
			if (capture)
			{
//				WINMGR().ReleaseCapture();
			}
//			return true;
		default:
			return false;
		}

		return true;
	}

	void Menu::OpenMenu(MenuItemPtr item)
	{
		CloseMenu();
		item->m_opened = true;
	}

	void Menu::CloseMenu()
	{
		m_active = nullptr;
		for (auto & item : m_items)
		{
			item->m_opened = false;
		}
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