#include "Color.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Widget.h"

namespace GUI
{
	Widget::Widget(const char* id) :
		m_id(id ? id : "")
	{
	}

	Widget::Widget(const char* id, RendererRef renderer, WidgetRef parent,
		Rect rect, const char* text, ImageRef image, FontRef font, CreationFlags flags) :
		m_id(id ? id : ""),
		m_renderer(renderer),
		m_parent(parent),
		m_rect(rect),
		m_text(text ? text : ""),
		m_image(image),
		m_showBorder(false),
		m_foregroundColor(Color::C_BLACK),
		m_backgroundColor(Color::C_LIGHT_GREY),
		m_borderColor(Color::C_BLACK),
		m_selectedBgColor(Color(90, 128, 128)),
		m_selectedFgColor(Color::C_WHITE),
		m_padding(0),
		m_margin(0),
		m_borderWidth(4),
		m_focused(false),
		m_flags(flags)
	{
		SetFont(font);
	}

	void Widget::SetParent(WidgetRef parent)
	{
		if (m_parent)
		{
			throw std::invalid_argument("widget already has parent");
		}
		m_parent = parent;
	}

	void Widget::SetFocus(WidgetRef focus, WidgetRef parent)
	{
		std::cout << "SetFocus: " << focus->GetId() << "(parent = " << (parent ? parent->GetId() : std::string("null")) << std::endl;
		if (parent == nullptr)
		{
			m_focused = true;
		}

		if (m_parent)
		{
			// Bubble up until we hit a window
			m_parent->SetFocus(focus, this);
		}
	}

	void Widget::SetFont(FontRef font)
	{
		if (font == nullptr)
		{
			m_font = RES().FindFont("default");
			if (m_font == nullptr)
			{
				throw std::exception("Unable to load default font");
			}
		}
		else
		{
			m_font = font;
		}
	}

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

	void Widget::DrawRect(const RectRef pos, const GUI::Color & col)
	{
		SetDrawColor(col);

		SDL_RenderDrawRect(m_renderer, pos);
	}

	void Widget::DrawFilledRect(const RectRef pos, const GUI::Color & col)
	{
		SetDrawColor(col);

		SDL_RenderFillRect(m_renderer, pos);
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

	void Widget::DrawButton(const RectRef pos, const GUI::Color & col, ImageRef image, bool raised, int thickness)
	{
		SetDrawColor(col);

		// Render rect
		SDL_RenderFillRect(m_renderer, pos);

		for (int i = 0; i < thickness; ++i)
		{
			Draw3dFrame(&pos->Deflate(i), raised);
		}
		
		if (image && image->IsSet())
		{
			image->Draw(&Point(pos->x + 1, pos->y + 1));
		}
	}

	TexturePtr Widget::SurfaceToTexture(SDL_Surface* surf)
	{
		TexturePtr texture = TexturePtr(SDL_CreateTextureFromSurface(m_renderer, surf), sdl_deleter());

		SDL_FreeSurface(surf);

		return std::move(texture);
	}

	Rect Widget::GetRect(bool relative, bool scrolled) const
	{
		if (m_parent == nullptr || (relative && !scrolled))
		{
			return m_rect;
		}

		Rect parent = m_parent->GetClientRect(relative, scrolled);
		return m_rect.Offset(&parent.Origin());
	}

	Rect Widget::GetClientRect(bool relative, bool scrolled) const
	{
		return GetRect(relative, scrolled);
	}

}