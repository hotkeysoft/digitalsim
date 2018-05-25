#include <SDL.h>
#include "ClipRect.h"
#include "Toolbar.h"
#include "Image.h"
#include "WindowManager.h"
#include "ToolbarItem.h"

namespace GUI
{
	Toolbar::Toolbar(RendererRef renderer, const char * id, int height) :
		Widget(id, renderer, nullptr, Rect(), nullptr), m_height(height)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}

		if (height < 8 || height > 128)
		{
			throw std::out_of_range("toolbar height out of range [8-128]");
		}

		m_padding = Dimension(2, 2);
		m_margin = 0;
	}

	ToolbarPtr Toolbar::Create(RendererRef renderer, const char * id, int height)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer, id, height);
		return std::static_pointer_cast<Toolbar>(ptr);
	}

	void Toolbar::Init()
	{
	}

	int Toolbar::GetHeight(int clientWidth) const
	{ 
		return m_height;
	}

	void Toolbar::Draw(const RectRef rect)
	{
		Rect drawRect = *rect;
		drawRect.h = GetHeight(rect->w);
		Draw3dFrame(&drawRect, true);

		drawRect = drawRect.Deflate(GetShrinkFactor());
		m_rect = drawRect;

		ClipRect clip(m_renderer, &drawRect);
		if (clip)
		{			
			for (auto & item : m_items)
			{
				Rect itemRect = item->GetRect(true, false);
				drawRect.w = itemRect.w;
				item->Draw(&drawRect);
				drawRect.x += itemRect.w;
			}
		}
	}

	ToolbarItemPtr Toolbar::AddToolbarItem(const char * id, ImageRef image, const char * name)
	{
		if (id == nullptr)
		{
			throw std::invalid_argument("id is null");
		}

		if (image)
		{
			Rect imageRect = image->GetRect(false, false);
			if ((imageRect.h + (2*m_borderWidth)) > m_height || imageRect.w > 128)
			{
				throw std::invalid_argument("image too large for toolbar");
			}
		}

		ToolbarItemPtr item = ToolbarItem::Create(id, m_renderer, name, image);
		item->SetParent(this);
		item->Init();
		
		m_items.push_back(item);

		return item;
	}

	HitResult Toolbar::HitTest(const PointRef pt)
	{		
		if (m_rect.PointInRect(pt))
		{
			return HitResult(HitZone::HIT_TOOLBAR, this);
		}

		return HitZone::HIT_NOTHING;
	}

	ToolbarItemPtr Toolbar::ItemAt(PointRef pt)
	{
		for (auto & item : m_items)
		{
			if (item->HitTest(pt))
			{
				return item;
			}
		}

		return nullptr;
	}

	bool Toolbar::HandleEvent(SDL_Event * e)
	{
		Point pt(e->button.x, e->button.y);
		HitResult hit = HitTest(&pt);
		if (hit)
		{
			ToolbarItemPtr item = ItemAt(&pt);
			if (item)
			{
				return item->HandleEvent(e);
			}
		}
		return false;
	}

	void Toolbar::SetFocus(WidgetRef focus, WidgetRef parent)
	{
		// Remove focus from other controls
		for (auto & control : m_items)
		{
			if (control.get() != parent)
			{
				control->ClearFocus();
			}
		}
	}

	Toolbar::ToolbarItems::const_iterator Toolbar::FindToolbarItem(ToolbarItemRef item) const
	{
		return std::find_if(
			m_items.begin(),
			m_items.end(),
			[item](ToolbarItemPtr it) { return it.get() == item; });
	}

	struct Toolbar::shared_enabler : public Toolbar
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Toolbar(std::forward<Args>(args)...)
		{
		}
	};
}