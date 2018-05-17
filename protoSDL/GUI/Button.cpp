#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Button.h"
#include <algorithm>

namespace GUI
{
	Button::Button(const char * id, RendererRef renderer, Rect rect, const char * label, ImageRef image, FontRef font) :
		Widget(id, renderer, nullptr, rect, label, image, font)
	{
	}

	void Button::Init()
	{
		RenderLabel();
	}

	ButtonPtr Button::Create(const char * id, RendererRef renderer, Rect rect, const char * label, ImageRef image, FontRef font)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, label, image, font);
		return std::static_pointer_cast<Button>(ptr);
	}

	HitZone Button::HitTest(const PointRef pt)
	{
		Rect parent = m_parent->GetClientRect(false, true);
		if (m_rect.Offset(&parent.Origin()).PointInRect(pt))
		{
			return HitZone::HIT_CONTROL;
		}
		
		return HitZone::HIT_NOTHING;
	}

	void Button::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect parent = m_parent->GetClientRect(false, true);
		Rect drawRect = m_rect.Offset(&parent.Origin());

		DrawButton(&drawRect, m_backgroundColor, nullptr, true);
		DrawLabel(&drawRect);
	}

	void Button::DrawLabel(RectRef rect)
	{
		if (m_labelText)
		{
			Rect buttonArea = rect->Deflate(m_borderWidth);

			// if (target > m_labelRect)
			Rect target = m_labelRect.CenterInTarget(&buttonArea);
			Rect source = { 0, 0, target.w, target.h };
			
			int deltaX = target.x - buttonArea.x;
			if (deltaX < 0) // Clip x
			{
				target.x = buttonArea.x;
				target.w = buttonArea.w;

				source.x -= deltaX;
				source.w = buttonArea.w;
			}

			int deltaY = target.y - buttonArea.y;
			if (deltaY < 0) // Clip y
			{
				target.y = buttonArea.y;
				target.h = buttonArea.h;

				source.y -= deltaY;
				source.h = buttonArea.h;
			}

			SDL_RenderCopy(m_renderer, m_labelText.get(), &source, &target);
		}
	}

	void Button::SetText(const char * label)
	{
		Widget::SetText(label);
		RenderLabel();
	}

	void Button::RenderLabel()
	{
		SDL_Surface* label = TTF_RenderText_Blended(m_font, m_text.c_str(), Color::C_BLACK);
		m_labelText = SurfaceToTexture(label);

		SDL_QueryTexture(m_labelText.get(), NULL, NULL, &m_labelRect.w, &m_labelRect.h);
		m_labelRect.x = 0;
		m_labelRect.y = 0;
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