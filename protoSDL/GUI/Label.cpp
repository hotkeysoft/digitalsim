#include "SDL.h"
#include "Point.h"
#include "ClipRect.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Label.h"
#include <algorithm>

namespace GUI
{
	Label::Label(const char * id, RendererRef renderer, Rect rect, const char * label, FontRef font, TextAlign align, CreationFlags flags) :
		Widget(id, renderer, nullptr, rect, label, nullptr, font, flags), m_labelAlign(align)
	{
		m_backgroundColor = Color::C_TRANSPARENT;
		m_borderWidth = 1;

		m_margin = (m_flags & WIN_FILL) ? 5 : 0;
	}

	void Label::Init()
	{
		RenderLabel();
	}

	void Label::SetForegroundColor(Color color)
	{
		bool colorChanged = (color != m_foregroundColor);
		Widget::SetForegroundColor(color);

		if (colorChanged)
		{
			RenderLabel();
		}
	}

	LabelPtr Label::CreateSingle(const char * id, RendererRef renderer, Rect rect, const char * label, FontRef font, TextAlign align)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, label, font, TEXT_SINGLE_DEFAULT, 0);
		return std::static_pointer_cast<Label>(ptr);
	}

	LabelPtr Label::CreateFill(const char * id, RendererRef renderer, const char * label, FontRef font, TextAlign align)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, Rect(), label, font, TEXT_FILL_DEFAULT, WIN_FILL);
		return std::static_pointer_cast<Label>(ptr);
	}

	LabelPtr Label::CreateAutoSize(const char* id, RendererRef renderer, Rect rect, const char* label, FontRef font, TextAlign align)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, label, font, TEXT_FILL_DEFAULT, WIN_AUTOSIZE);
		return std::static_pointer_cast<Label>(ptr);
	}

	Rect Label::GetRect(bool relative, bool scrolled) const
	{
		Rect parent = m_parent->GetClientRect(relative, scrolled);

		return m_rect.Offset(&parent.Origin());
	}

	Rect Label::GetClientRect(bool relative, bool scrolled) const
	{
		return GetRect(relative, scrolled);
	}

	void Label::DrawBackground(const GUI::RectRef &rect)
	{
		if (m_backgroundColor.IsTransparent())
			return;

		Rect fillRect = m_rect;
		fillRect.h = std::max(rect->h, m_rect.h);
		fillRect.w = std::max(rect->w, m_rect.w);

		DrawFilledRect(rect, m_backgroundColor);
	}

	Rect Label::DrawFrame(const RectRef &rect)
	{
		Rect frameRect = *rect;

		if (m_flags & WIN_FILL)
		{
			DrawBackground(&m_parent->GetClientRect(false, true));
		}

		if (m_margin)
		{
			frameRect = frameRect.Deflate(m_margin);
		}

		if (!(m_flags & WIN_FILL) && !m_backgroundColor.IsTransparent())
		{
			DrawFilledRect(&frameRect, m_backgroundColor);
		}

		if (m_showBorder)
		{
			for (int i = 0; i < m_borderWidth; ++i)
			{
				DrawRect(&frameRect, m_borderColor);
				frameRect = frameRect.Deflate(1);
			}
		}

		return std::move(frameRect);
	}

	void Label::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect drawRect;
		Rect frameRect;
		if (m_flags & WIN_FILL)
		{
			frameRect = DrawFrame(&m_parent->GetClientRect(false, false));
			drawRect = m_parent->GetClientRect(false, true);
		}
		else
		{
			drawRect = GetRect(false, true);
			frameRect = DrawFrame(&drawRect);
		}

		drawRect = drawRect.Deflate(GetShrinkFactor());

		ClipRect clip(m_renderer, &frameRect);
		if (clip)	
		{
			DrawLabel(&drawRect);
		}
	}

	void Label::Draw(const RectRef rect)
	{
		Rect frameRect = DrawFrame(rect);

		Rect drawFrame = rect->Deflate(GetShrinkFactor());

		ClipRect clip(m_renderer, &drawFrame);
		if (clip)
		{
			DrawLabel(rect);
		}
	}

	void Label::DrawLabel(RectRef rect)
	{
		if (m_labelText)
		{
			bool hCenter = (m_labelAlign & TEXT_H_CENTER) == TEXT_H_CENTER;
			bool vCenter = (m_labelAlign & TEXT_V_CENTER) == TEXT_V_CENTER;

			Rect target = m_labelRect.CenterInTarget(rect, hCenter, vCenter);
			Rect source = { 0, 0, target.w, target.h };
			
			int deltaX = target.x - rect->x;
			if (deltaX < 0) // Clip x
			{
				target.x = rect->x;
				target.w = rect->w;

				source.x -= deltaX;
				source.w = rect->w;
			}

			int deltaY = target.y - rect->y;
			if (deltaY < 0) // Clip y
			{
				target.y = rect->y;
				target.h = rect->h;

				source.y -= deltaY;
				source.h = rect->h;
			}

			if ((m_labelAlign & TEXT_H_CENTER) == TEXT_H_RIGHT)
			{
				target.x += rect->w - source.w;
			}

			if ((m_labelAlign & TEXT_V_CENTER) == TEXT_V_BOTTOM)
			{
				target.y += rect->h - source.h;
			}

			SDL_RenderCopy(m_renderer, m_labelText.get(), &source, &target);
		}
	}

	void Label::SetText(const char * label)
	{
		Widget::SetText(label);
		RenderLabel();
	}

	void Label::RenderLabel()
	{
		SDL_Surface* label = TTF_RenderText_Blended(m_font, m_text.c_str(), m_foregroundColor);
		m_labelText = SurfaceToTexture(label);

		SDL_QueryTexture(m_labelText.get(), NULL, NULL, &m_labelRect.w, &m_labelRect.h);
		m_labelRect.x = 0;
		m_labelRect.y = 0;
		
		if (m_flags & WIN_AUTOSIZE)
		{
			m_rect.w = m_labelRect.w + (2 * GetShrinkFactor().w);
			m_rect.h = m_labelRect.h + (2 * GetShrinkFactor().h);
		}
	}

	void Label::SetAlign(uint8_t align)
	{
		// Fill in with default if H or V alignment is not specified
		if ((align & TEXT_H_CENTER) == 0)
		{
			align |= TEXT_H_CENTER;
		}

		if ((align & TEXT_V_CENTER) == 0)
		{
			align |= TEXT_V_CENTER;
		}

		m_labelAlign = (TextAlign)align;
	}

	struct Label::shared_enabler : public Label
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Label(std::forward<Args>(args)...)
		{
		}
	};
}