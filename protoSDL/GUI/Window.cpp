#include "SDL.h"
#include "SDL_rect.h"
#include "Window.h"
#include "Image.h"
#include <algorithm>

namespace GUI
{
	ImagePtr Window::m_titleBackground = nullptr;

	Window Window::m_nullWnd;

	Window::Window() : m_id("null"), m_renderer(nullptr), m_parent(nullptr), m_image(nullptr), m_font(nullptr), m_visible(false) {}

	Window::Window(const char* id, RendererRef renderer, WindowRef parent, FontRef font, SDL_Rect rect, WindowCreationFlags flags) : m_renderer(renderer),
		m_parent(parent), m_image(nullptr), m_font(font), m_rect(rect), m_visible(true), m_flags(flags)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
		if (id == nullptr)
		{
			throw std::invalid_argument("id is null");
		}
		m_id = id;
	}

	WindowPtr Window::Create(const char* id, RendererRef renderer, WindowRef parent, FontRef font, SDL_Rect rect, WindowCreationFlags flags)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, parent, font, rect, flags);
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

	WindowManager::WindowList Window::GetChildWindows()
	{
		return WINMGR().GetWindowList(this);
	}

	SDL_Rect Window::GetClientRect(bool relative) const
	{
		SDL_Rect rect = GetWindowRect(relative);
		if (relative)
		{
			rect.x = 0;
			rect.y = 0;
		}
		else
		{
			rect.x += m_borderWidth + 1;
			rect.y += m_borderWidth + (m_buttonSize + 2) + 1;
		}
		rect.w -= (2 * (m_borderWidth + 1));
		rect.h -= (2 * (m_borderWidth + 1)) + (m_buttonSize + 2);
		
		return rect;
	}

	SDL_Rect Window::GetTitleBarRect() const
	{
		SDL_Rect rect = GetWindowRect(false);
		rect.x += (m_buttonSize + 2) + m_borderWidth + 1;
		rect.y += m_borderWidth + 1;
		rect.w -= (2 * (m_borderWidth + 1)) + (m_buttonSize + 2);
		rect.h = m_buttonSize + 2;
		return rect;
	}

	SDL_Rect Window::GetWindowRect(bool relative) const
	{
		SDL_Rect rect = m_rect;
		if (!relative && m_parent != nullptr)
		{
			SDL_Rect parentClient = m_parent->GetClientRect(false);
			rect.x += parentClient.x;
			rect.y += parentClient.y;
		}
		
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
		TexturePtr title;
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
			SDL_Rect target = GetTitleBarRect();

			target.x += m_borderWidth;
			target.y += m_borderWidth / 2;
			target.w = std::min(m_titleStrRect.w, target.w - (2 * m_borderWidth));
			target.h = std::min(m_titleStrRect.h, target.h);

			SDL_Rect source = { 0, 0, target.w, target.h };
			SDL_RenderCopy(m_renderer, title.get(), &source, &target);
		}
	}

	HitZone Window::HitTest(SDL_Point pt)
	{
		if (SDL_PointInRect(&pt, &GetTitleBarRect()))
		{
			return HIT_TITLEBAR;
		}
		else if (SDL_PointInRect(&pt, &GetClientRect(false)))
		{
			return HIT_CLIENT;
		}

		SDL_Rect wndRect = GetWindowRect(false);
		if (!SDL_PointInRect(&pt, &wndRect))
		{
			return HIT_NOTHING;
		}

		bool left = pt.x < wndRect.x + 2*m_borderWidth;
		bool top = pt.y < wndRect.y + 2*m_borderWidth;
		bool right = pt.x > wndRect.x + wndRect.w - 2*m_borderWidth;
		bool bottom = pt.y > wndRect.y + wndRect.h - 2*m_borderWidth;
		if (top)
		{
			if (left) return HIT_CORNER_TOPLEFT;
			else if (right) return HIT_CORNER_TOPRIGHT;
			else return HIT_BORDER_TOP;
		}
		else if (bottom)
		{
			if (left) return HIT_CORNER_BOTTOMLEFT;
			else if (right) return HIT_CORNER_BOTTOMRIGHT;
			else return HIT_BORDER_BOTTOM;
		}
		else return left? HIT_BORDER_LEFT : HIT_BORDER_RIGHT;
	}

	SDL_Rect GetClipRect(WindowRef win)
	{
		SDL_Rect rect = win->GetClientRect(false);

		while (win)
		{
			SDL_IntersectRect(&rect, &win->GetClientRect(false), &rect);
			win = win->GetParent();
		}

		return rect;
	}

	void Window::Draw()
	{
		if (m_parent != nullptr)
		{
			SDL_Rect clip = GetClipRect(m_parent);
			SDL_RenderSetClipRect(m_renderer, &clip);
		}

		SDL_Rect rect = GetWindowRect(false);
		bool active = (WINMGR().GetActive() == this);
		DrawReliefBox(rect, Color::C_LIGHT_GREY, false);
		DrawTitleBar(active);
		DrawTitle(active);
		DrawButton(rect, Color::C_LIGHT_GREY, true);

		SDL_RenderSetClipRect(m_renderer, nullptr);
	}
	
	void DeleteTexture(SDL_Texture* surface)
	{
		SDL_DestroyTexture(surface);
	}

	TexturePtr Window::SurfaceToTexture(SDL_Surface* surf)
	{
		TexturePtr texture = TexturePtr(SDL_CreateTextureFromSurface(m_renderer, surf), DeleteTexture);

		SDL_FreeSurface(surf);

		return std::move(texture);
	}

	void Window::RenderTitle()
	{
		SDL_Surface* active = TTF_RenderText_Blended(m_font, m_title.c_str(), GUI::Color::C_WHITE);
		m_activeTitle = SurfaceToTexture(active);

		SDL_Surface* inactive = TTF_RenderText_Blended(m_font, m_title.c_str(), GUI::Color::C_DARK_GREY);
		m_inactiveTitle = SurfaceToTexture(inactive);

		SDL_QueryTexture(m_activeTitle.get(), NULL, NULL, &m_titleStrRect.w, &m_titleStrRect.h);
		m_titleStrRect.x = 0;
		m_titleStrRect.y = 0;
	}

	bool Window::MoveRel(SDL_Point rel)
	{
		bool clip = false;
		if (m_flags & WIN_CANMOVE)
		{
			m_rect.x += rel.x;
			m_rect.y += rel.y;

			if (m_rect.x < 0)
			{
				clip = true;
				m_rect.x = 0;
			}

			if (m_rect.y < 0)
			{
				clip = true;
				m_rect.y = 0;
			}
		}		

		return !clip;
	}

	bool Window::ResizeRel(SDL_Point rel)
	{
		bool clip = false;
		if (m_flags & WIN_CANRESIZE)
		{
			m_rect.w += rel.x;
			m_rect.h += rel.y;

			if (m_rect.w < 100)
			{
				clip = true;
				m_rect.w = 100;
			}

			if (m_rect.h < 100)
			{
				clip = true;
				m_rect.h = 100;
			}
		}
		return clip;
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