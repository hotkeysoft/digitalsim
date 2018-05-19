#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Button.h"
#include "Label.h"
#include <algorithm>

namespace GUI
{
	Button::Button(const char * id, RendererRef renderer, Rect rect, const char * label, ImageRef image, FontRef font) :
		Widget(id, renderer, nullptr, rect, label, image, font), m_pushed(false), m_onClickHandler(nullptr)
	{
	}

	void Button::Init()
	{
		if (m_text.size())
		{
			CreateLabel();
		}
	}

	ButtonPtr Button::Create(const char * id, RendererRef renderer, Rect rect, const char * label, ImageRef image, FontRef font)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, label, image, font);
		return std::static_pointer_cast<Button>(ptr);
	}

	bool Button::HandleEvent(SDL_Event *e)
	{
		Point pt(e->button.x, e->button.y);
		HitResult hit = HitTest(&pt);
		const CaptureInfo & capture = WINMGR().GetCapture();
		switch (e->type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (hit.target == this)
			{
				m_pushed = true;
				WINMGR().StartCapture(hit, &pt);
				return true;
			}
		case SDL_MOUSEBUTTONUP:
			m_pushed = false;
			if (capture && capture.Target.target == this && hit.target == this)
			{
				WINMGR().ReleaseCapture();
				if (m_onClickHandler)
				{
					m_onClickHandler(this);
				}
			}
			return true;
		case SDL_MOUSEMOTION:
			if (capture && capture.Target.target == this)
			{
				m_pushed = (hit.target == this);
			}
		}
		return false;
	}

	HitResult Button::HitTest(const PointRef pt)
	{
		Rect parent = m_parent->GetClientRect(false, true);
		if (m_rect.Offset(&parent.Origin()).PointInRect(pt))
		{
			return HitResult(HitZone::HIT_CONTROL, this);
		}
		
		return HitZone::HIT_NOTHING;
	}

	Rect Button::GetRect(bool relative, bool scrolled) const
	{
		Rect parent = m_parent->GetClientRect(relative, scrolled);

		return m_rect.Offset(&parent.Origin());
	}

	Rect Button::GetClientRect(bool relative, bool scrolled) const
	{
		return GetRect(relative, scrolled).Deflate(m_borderWidth);
	}

	void Button::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect drawRect = GetRect(false, true);

		DrawButton(&drawRect, m_backgroundColor, nullptr, !m_pushed);

		if (m_label)
		{
			m_label->Draw();
		}
	}

	void Button::SetText(const char * label)
	{
		Widget::SetText(label);
		CreateLabel();
	}

	void Button::CreateLabel()
	{
		m_label = Label::CreateFill("label", m_renderer, m_text.c_str(), m_font);
		m_label->SetParent(this);
		m_label->Init();
	}

	struct Button::shared_enabler : public Button
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Button(std::forward<Args>(args)...)
		{
		}
	};
}