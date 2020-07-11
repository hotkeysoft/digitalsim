#include "stdafx.h"
#include "SDL.h"
#include "Core/Point.h"
#include "Core/Rect.h"
#include "Core/Window.h"
#include "Core/ResourceManager.h"
#include "Widgets/Label.h"
#include "Component.h"
#include <algorithm>

namespace CoreUI
{
	Component::Component(const char * id, RendererRef renderer, Rect rect, const char* label, ImageRef image, FontRef font, CreationFlags flags) :
	Widget(id, renderer, nullptr, rect, label, image, font, flags)
	{
	}

	void Component::Init()
	{
		if (m_text.size())
		{
			CreateLabel();
		}
	}

	ComponentPtr Component::Create(const char * id, RendererRef renderer, Rect rect, const char* label, ImageRef image, FontRef font, CreationFlags flags)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, label, image, font, flags);
		return std::static_pointer_cast<Component>(ptr);
	}

	bool Component::HandleEvent(SDL_Event *e)
	{
		Point pt(e->button.x, e->button.y);
		HitResult hit = HitTest(&pt);
		const CaptureInfo & capture = WINMGR().GetCapture();
		switch (e->type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (hit.target == this)
			{
				SetActive();
				SetFocus(this);
				WINMGR().StartCapture(hit, &pt);
				return true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (capture && capture.Target.target == this)
			{
				WINMGR().ReleaseCapture();
				if (hit.target == this)
				{
				}
			}
			return true;
		case SDL_MOUSEMOTION:
			if (hit)
			{
				SDL_SetCursor(RES().FindCursor("default"));
			}

			if (capture && capture.Target.target == this)
			{
				Point newPos = pt;
				newPos.x += capture.Delta.x;
				newPos.y += capture.Delta.y;
				Point delta = { (capture.Origin.x - newPos.x) , (capture.Origin.y - newPos.y) };

				bool handled = true;
				MovePos(&GetParentWnd()->GetGrid().Snap(&newPos));

				if (handled)
					return handled;
			}
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		}
		return false;
	}

	HitResult Component::HitTest(const PointRef pt)
	{
		Rect parent = m_parent->GetClientRect(false, true);
		if (m_rect.Offset(&parent.Origin()).PointInRect(pt))
		{
			return HitResult(HitZone::HIT_CONTROL, this);
		}
		
		return HitZone::HIT_NOTHING;
	}

	Rect Component::GetClientRect(bool relative, bool scrolled) const
	{
		return GetRect(relative, scrolled).Deflate(m_borderWidth);
	}

	void Component::Draw()
	{
		if (m_parent == nullptr)
			return;

		m_borderWidth = IsFocused() ? 2 : 1;

		Rect drawRect = GetRect(false, true);

		DrawFilledRect(&drawRect, m_backgroundColor);

		DrawRect(&drawRect, m_foregroundColor, m_borderWidth);	

		if (m_label)
		{
			m_label->SetBorder(false);
			m_label->Draw(&drawRect.Deflate(m_borderWidth));
		}
	}

	void Component::SetText(const char * label)
	{
		Widget::SetText(label);
		CreateLabel();
	}

	void Component::CreateLabel()
	{
		m_label = Label::CreateFill("label", m_renderer, m_text.c_str(), m_font);

		m_label->SetMargin(Dimension(5, 2));
		m_label->SetPadding(0);
		m_label->SetBorderColor(Color::C_MED_GREY);
		m_label->SetBorderWidth(1);
		m_label->SetBorder(true);
		m_label->SetParent(this);
		m_label->Init();
	}

	struct Component::shared_enabler : public Component
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Component(std::forward<Args>(args)...)
		{
		}
	};
}