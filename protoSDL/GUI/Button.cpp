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
		Widget(id, renderer, nullptr, rect, label, image, font)
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

	HitResult Button::HitTest(const PointRef pt)
	{
		Rect parent = m_parent->GetClientRect(false, true);
		if (m_rect.Offset(&parent.Origin()).PointInRect(pt))
		{
			return std::make_tuple(HitZone::HIT_CONTROL, this);
		}
		
		return std::make_tuple(HitZone::HIT_NOTHING, nullptr);
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

		DrawButton(&drawRect, m_backgroundColor, nullptr, true);

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
		m_label = Label::Create("label", m_renderer, { 0, 0, m_rect.w, m_rect.h }, m_text.c_str(), m_font);
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