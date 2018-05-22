#include <SDL.h>
#include "ClipRect.h"
#include "Menu.h"
#include "Label.h"
#include "MenuItem.h"

namespace GUI
{
	Menu::Menu(RendererRef renderer) : Widget("Menu", renderer, nullptr, Rect(), nullptr)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}

		m_padding = 2;
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
		drawRect.h = GetHeight();

		Draw3dFrame(&drawRect, true);

		drawRect = drawRect.Deflate(GetShrinkFactor());
		m_rect = drawRect;

		//TODO: Get main window size
		ClipRect clip(m_renderer, &Rect(0,0,1000,1000), false);
		if (clip)
		{
			for (auto item : m_items)
			{
				Rect labelRect = item->GetLabel()->GetRect(true, false);				
				drawRect.w = labelRect.w;

				if (item->IsOpened())
				{
					Draw3dFrame(&drawRect, false);
					Rect rect(drawRect);
					rect.y += (labelRect.h);
					rect.w = 200;
					rect.h = 200;
					DrawFilledRect(&rect, m_backgroundColor);
					Draw3dFrame(&rect, true);

					rect.h = labelRect.h;
					DrawButton(&rect, m_backgroundColor, nullptr, true, 1);
					rect.y += labelRect.h;
					DrawButton(&rect, m_backgroundColor, nullptr, true, 1);
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

		MenuItemPtr item = MenuItem::Create(m_renderer, id, name);
		item->Init();

		m_items.push_back(item);

		return item;
	}

	HitResult Menu::HitTest(const PointRef pt)
	{		
		if (m_rect.PointInRect(pt))
		{
			return HitResult(HitZone::HIT_MENU, this);
		}

		return HitZone::HIT_NOTHING;
	}

	MenuItemPtr Menu::ItemAt(PointRef pt)
	{
		auto it = std::find_if(m_items.begin(), m_items.end(), [pt](MenuItemPtr item) { return item->Hit(pt); });
		if (it != m_items.end())
		{
			return *it;
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
					if (item != nullptr)
					{
						OpenMenu(item);
					}
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			MenuItemPtr item = ItemAt(&pt);

			if (capture)
			{
				CloseMenu();
				WINMGR().ReleaseCapture();
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
		for (auto item : m_items)
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