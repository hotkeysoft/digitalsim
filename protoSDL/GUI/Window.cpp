#include "SDL.h"
#include "SDL_rect.h"
#include "Window.h"
#include "Image.h"

namespace GUI
{
	ImagePtr Window::m_titleBackground = nullptr;

	Window::Window(SDL::RendererRef renderer, SDL::FontRef font, SDL_Rect rect) : m_renderer(renderer), m_image(nullptr), m_font(font), m_rect(rect), m_visible(true)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
	}

	WindowPtr Window::Create(SDL::RendererRef renderer, SDL::FontRef font, SDL_Rect rect)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer, font, rect);
		return std::static_pointer_cast<Window>(ptr);
	}

	void Window::SetTitle(const char * title)
	{
		if (title == nullptr)
		{
			m_title = "";
		}
		else
		{
			m_title = title;
		}
		RenderTitle();
	}

	void Window::SetImage(ImageRef image)
	{
		m_image = image;
	}

	void Window::SetDrawColor(const GUI::Color & col)
	{
		SDL_SetRenderDrawColor(m_renderer, col.r, col.g, col.b, col.a);
	}

	void Window::Draw3dFrame(SDL_Rect pos, bool raised)
	{
		// Relief effect
		SDL_Point points[5] = { { pos.x, pos.y + pos.h - 1 },{ pos.x + pos.w - 1, pos.y + pos.h - 1 },
		{ pos.x + pos.w - 1, pos.y },{ pos.x, pos.y },{ pos.x, pos.y + pos.h - 2 } };

		SetDrawColor(raised ? Color::C_DARK_GREY : Color::C_WHITE);
		SDL_RenderDrawLines(m_renderer, points + 0, 3);

		SetDrawColor(raised ? Color::C_WHITE : Color::C_DARK_GREY);
		SDL_RenderDrawLines(m_renderer, points + 2, 3);
	}

	void Window::DrawReliefBox(SDL_Rect pos, const GUI::Color & col, bool raised)
	{
		SetDrawColor(col);

		// Render rect
		SDL_RenderFillRect(m_renderer, &pos);

		Draw3dFrame(pos, !raised);

		if (m_borderWidth > 1)
		{
			pos.x += m_borderWidth;
			pos.y += m_borderWidth;
			pos.w -= (2 * m_borderWidth);
			pos.h -= (2 * m_borderWidth);
			Draw3dFrame(pos, raised);
		}
	}

	void Window::DrawButton(SDL_Rect pos, const GUI::Color & col, bool raised)
	{
		pos.x += m_borderWidth + 1;
		pos.y += m_borderWidth + 1;
		pos.w = m_buttonSize + 2;
		pos.h = m_buttonSize + 2;

		SetDrawColor(col);

		// Render rect
		SDL_RenderFillRect(m_renderer, &pos);

		Draw3dFrame(pos, raised);

		if (m_image && m_image->IsSet())
		{
			m_image->Draw(SDL_Point({pos.x+1, pos.y+1}));
		}
	}

	SDL_Rect Window::GetClientRect() const
	{
		SDL_Rect rect = m_rect;
		rect.x += m_borderWidth + 1;
		rect.y += m_borderWidth + (m_buttonSize + 2) + 1;
		rect.w -= (2 * (m_borderWidth + 1));
		rect.h -= (2 * (m_borderWidth + 1)) + (m_buttonSize + 2);
		
		return rect;
	}

	SDL_Rect Window::GetTitleBarRect() const
	{
		SDL_Rect rect = m_rect;
		rect.x += (m_buttonSize + 2) + m_borderWidth + 1;
		rect.y += m_borderWidth + 1;
		rect.w -= (2 * (m_borderWidth + 1)) + (m_buttonSize + 2);
		rect.h = m_buttonSize + 2;
		return rect;
	}

	void Window::DrawTitleBar(bool active)
	{	
		auto rect = GetTitleBarRect();
		if (active)
		{
			if (m_titleBackground == nullptr)
			{
				m_titleBackground = Image::Create(m_renderer);
				m_titleBackground->LoadFromFile("Resources/TitlebarActive.png");				
			}
			m_titleBackground->Draw(rect);
		}
		else
		{
			SetDrawColor(Color::C_LIGHT_GREY);
			SDL_RenderFillRect(m_renderer, &rect);
			Draw3dFrame(rect, true);
		}
	}

	void Window::DrawTitle(bool active)
	{
		SDL::TexturePtr title;
		if (active && m_activeTitle)
		{
			title = m_activeTitle;
		}
		else if (!active && m_inactiveTitle)
		{
			title = m_inactiveTitle;
		}

		if (title)
		{
			SDL_Rect source = { 0, 0, m_titleStrRect.w, m_titleStrRect.h };
			SDL_RenderCopy(m_renderer, title.get(), &source, &m_titleStrRect);
		}
	}

	Window::HitZone Window::HitTest(SDL_Point pt)
	{
		if (SDL_PointInRect(&pt, &GetTitleBarRect()))
		{
			return HIT_TITLEBAR;
		}
		else if (SDL_PointInRect(&pt, &GetClientRect()))
		{
			return HIT_CLIENT;
		}
		else if (SDL_PointInRect(&pt, &m_rect))
		{
			return HIT_BORDER_ANY;
		}

		return HIT_NOTHING;
	}

	void Window::Draw(bool active)
	{
		DrawReliefBox(m_rect, Color::C_LIGHT_GREY, false);
		DrawTitleBar(active);
		DrawTitle(active);
		DrawButton(m_rect, Color::C_LIGHT_GREY, true);
	}
	
	void DeleteTexture(SDL_Texture* surface)
	{
		SDL_DestroyTexture(surface);
	}

	SDL::TexturePtr Window::SurfaceToTexture(SDL_Surface* surf)
	{
		SDL::TexturePtr texture = SDL::TexturePtr(SDL_CreateTextureFromSurface(m_renderer, surf), DeleteTexture);

		SDL_FreeSurface(surf);

		return std::move(texture);
	}

	void Window::RenderTitle()
	{
		SDL_Surface* active = TTF_RenderText_Blended(m_font, m_title.c_str(), GUI::Color::C_WHITE);
		m_activeTitle = SurfaceToTexture(active);

		SDL_Surface* inactive = TTF_RenderText_Blended(m_font, m_title.c_str(), GUI::Color::C_DARK_GREY);
		m_inactiveTitle = SurfaceToTexture(inactive);

		int fontW, fontH;
		SDL_QueryTexture(m_activeTitle.get(), NULL, NULL, &fontW, &fontH);
		m_titleStrRect = GetTitleBarRect();
		m_titleStrRect.x += m_borderWidth;
		m_titleStrRect.w -= (2 * m_borderWidth);
		m_titleStrRect.y += m_borderWidth/2;
		if (fontW < m_titleStrRect.w)
		{
			m_titleStrRect.w = fontW;
		}
		if (fontH < m_titleStrRect.h)
		{
			m_titleStrRect.h = fontH;
		}
	}

	struct Window::shared_enabler : public Window
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Window(std::forward<Args>(args)...)
		{
		}
	};
}