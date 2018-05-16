#include "Color.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Widget.h"

namespace GUI
{

	void Widget::SetDrawColor(const GUI::Color & col)
	{
		SDL_SetRenderDrawColor(m_renderer, col.r, col.g, col.b, col.a);
	}

	void Widget::Draw3dFrame(const RectRef pos, bool raised)
	{
		// Relief effect
		Point points[5] = { { pos->x, pos->y + pos->h - 1 },{ pos->x + pos->w - 1, pos->y + pos->h - 1 },
		{ pos->x + pos->w - 1, pos->y },{ pos->x, pos->y },{ pos->x, pos->y + pos->h - 2 } };

		SetDrawColor(raised ? Color::C_DARK_GREY : Color::C_WHITE);
		SDL_RenderDrawLines(m_renderer, points + 0, 3);

		SetDrawColor(raised ? Color::C_WHITE : Color::C_DARK_GREY);
		SDL_RenderDrawLines(m_renderer, points + 2, 3);
	}

	void Widget::DrawReliefBox(const RectRef pos, const GUI::Color & col, bool raised)
	{
		SetDrawColor(col);

		// Render rect
		SDL_RenderFillRect(m_renderer, pos);

		Draw3dFrame(pos, !raised);

		if (m_borderWidth > 1)
		{
			Rect frame(pos->x + m_borderWidth,
				pos->y + m_borderWidth,
				pos->w - (2 * m_borderWidth),
				pos->h - (2 * m_borderWidth));
			Draw3dFrame(&frame, raised);
		}
	}

	void Widget::DrawButton(const RectRef pos, const GUI::Color & col, ImageRef image, bool raised)
	{
		SetDrawColor(col);

		// Render rect
		SDL_RenderFillRect(m_renderer, pos);

		Draw3dFrame(pos, raised);

		if (image && image->IsSet())
		{
			image->Draw(&Point(pos->x + 1, pos->y + 1));
		}
	}

}