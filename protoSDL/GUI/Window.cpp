#include "SDL.h"
#include "Window.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "ResourceManager.h"
#include <algorithm>

namespace GUI
{
	ImagePtr Window::m_titleBackground = nullptr;

	Window Window::m_nullWnd;

	Window::Window() : Widget(nullptr), m_id("null"), m_parent(nullptr), m_image(nullptr), m_font(nullptr), 
		m_showState(), m_pushedState(HIT_NOTHING) {}

	Window::Window(const char* id, RendererRef renderer, WindowRef parent, FontRef font, Rect rect, WindowFlags flags) : 
		Widget(renderer), m_parent(parent), m_image(nullptr), m_font(font), 
		m_rect(rect), m_showState(WindowState::WS_VISIBLE), m_flags(flags), m_pushedState(HIT_NOTHING)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
		if (id == nullptr)
		{
			throw std::invalid_argument("id is null");
		}

		if (m_flags & WindowFlags::WIN_MINMAX && m_parent == nullptr)
		{
			throw std::invalid_argument("WIN_MINMAX flag needs parent window");
		}

		m_id = id;

		m_scrollBars = ScrollBars::Create(renderer, this);
	}

	WindowPtr Window::Create(const char* id, RendererRef renderer, WindowRef parent, FontRef font, Rect rect, WindowFlags flags)
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

	void Window::DrawSystemMenuButton(Rect pos, const GUI::Color & col)
	{
		DrawButton(&GetSystemMenuButtonRect(pos), col, m_image, !(m_pushedState & HIT_SYSMENU));
	}

	void Window::DrawMinMaxButtons(Rect pos, const GUI::Color & col)
	{
		const char * resStr = (m_showState & WindowState::WS_MINIMIZED) ? "win.restore" : "win.minimize";
		DrawButton(&GetMinimizeButtonRect(pos), col, RES().FindImage(resStr), !(m_pushedState & HIT_MINBUTTON));

		resStr = (m_showState & WindowState::WS_MAXIMIZED) ? "win.restore" : "win.maximize";

		DrawButton(&GetMaximizeButtonRect(pos), col, RES().FindImage(resStr), !(m_pushedState & HIT_MAXBUTTON));
	}

	ScrollState Window::GetScrollBarState() const
	{
		ScrollState ret;

		if (m_parent != nullptr && !(m_showState & (WindowState::WS_MAXIMIZED | WindowState::WS_MINIMIZED)))
		{
			Rect thisRect = GetWindowRect(true, false);
			Rect parentRect = m_parent->GetClientRect(true);
		
			ret.showH = ((thisRect.x + thisRect.w) > parentRect.w);
			ret.showV = ((thisRect.y + thisRect.h) > parentRect.h);
			ret.hMax = ((thisRect.x + thisRect.w) - parentRect.w);
			ret.vMax = ((thisRect.y + thisRect.h) - parentRect.h);
		}
		return ret;
	}

	WindowManager::WindowList Window::GetChildWindows()
	{
		return WINMGR().GetWindowList(this);
	}

	Rect Window::GetClientRect(bool relative) const
	{
		Rect rect = GetWindowRect(relative, false);
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

		ScrollStateRef scroll = m_scrollBars->GetScrollState();
		auto size = m_scrollBars->GetSize();

		rect.w -= (2 * (m_borderWidth + 1)) + (scroll->showV ? size : 0);
		rect.h -= (2 * (m_borderWidth + 1)) + (m_buttonSize + 2) + (scroll->showH ? size : 0);

		return rect;
	}

	Rect Window::GetTitleBarRect(Rect rect) const
	{
		int sysButtonW = (m_flags & WindowFlags::WIN_SYSMENU) ? m_buttonSize + 2 : 0;
		int minMaxButtonW = (m_flags & WindowFlags::WIN_MINMAX) ? (m_buttonSize + 2) * 2 : 0;		

		rect.x += sysButtonW + m_borderWidth + 1;
		rect.y += m_borderWidth + 1;
		rect.w -= (2 * (m_borderWidth + 1)) + sysButtonW + minMaxButtonW;
		rect.h = m_buttonSize + 2;
		return rect;
	}

	Rect Window::GetSystemMenuButtonRect(Rect rect) const
	{
		rect.x += m_borderWidth + 1;
		rect.y += m_borderWidth + 1;
		rect.w = m_buttonSize + 2;
		rect.h = m_buttonSize + 2;

		return rect;
	}

	Rect Window::GetMinimizeButtonRect(Rect rect) const
	{
		rect.x += rect.w - (m_borderWidth + (m_buttonSize + 2) * 2 + 1);
		rect.y += m_borderWidth + 1;

		rect.w = m_buttonSize + 2;
		rect.h = m_buttonSize + 2;

		return rect;
	}

	Rect Window::GetMaximizeButtonRect(Rect rect) const
	{
		rect.x += rect.w - (m_borderWidth + (m_buttonSize + 2) + 1);
		rect.y += m_borderWidth + 1;

		rect.w = m_buttonSize + 2;
		rect.h = m_buttonSize + 2;

		return rect;
	}

	Rect Window::GetWindowRect(bool relative, bool scrolled) const
	{
		Rect rect = m_rect;
		if (m_showState & WindowState::WS_MAXIMIZED)
		{
			rect = m_parent->GetClientRect(true);
		}
		else if (m_showState & WindowState::WS_MINIMIZED)
		{
			int minimizedHeight = (m_buttonSize + 2) + (2 * m_borderWidth) + 2;
			rect = m_parent->GetClientRect(true);
			rect.w = 200;			
			rect.x = m_parent->GetMinimizedChildIndex(const_cast<WindowRef>(this)) * 200;
			rect.y = (rect.h - minimizedHeight);
			rect.h = minimizedHeight;
		}

		if (m_parent != nullptr)
		{
			if (!relative)
			{
				Rect parentClient = m_parent->GetClientRect(false);
				rect.x += parentClient.x;
				rect.y += parentClient.y;
			}

			if (scrolled)
			{
				PointRef scrollOffset = m_parent->GetScroll();
				rect.x -= scrollOffset->x;
				rect.y -= scrollOffset->y;
			}
		}
	
		return rect;
	}

	void Window::DrawTitleBar(Rect rect, bool active)
	{	
		auto titleBar = GetTitleBarRect(rect);
		if (active)
		{
			if (m_titleBackground == nullptr)
			{
				m_titleBackground = Image::Create(m_renderer);
				m_titleBackground->LoadFromFile("Resources/TitlebarActive.png");				
			}
			m_titleBackground->Draw(&titleBar);
		}
		else
		{
			SetDrawColor(Color::C_LIGHT_GREY);
			SDL_RenderFillRect(m_renderer, &titleBar);
			Draw3dFrame(&titleBar, true);
		}
	}

	void Window::DrawTitle(Rect rect, bool active)
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
			Rect target = GetTitleBarRect(rect);

			target.x += m_borderWidth;
			target.y += m_borderWidth / 2;
			target.w = std::min(m_titleStrRect.w, target.w - (2 * m_borderWidth));
			target.h = std::min(m_titleStrRect.h, target.h);

			Rect source = { 0, 0, target.w, target.h };
			SDL_RenderCopy(m_renderer, title.get(), &source, &target);
		}
	}

	HitZone Window::HitTest(const PointRef pt)
	{
		if (!(m_showState & WindowState::WS_VISIBLE))
		{
			return HIT_NOTHING;
		}

		Rect wndRect = GetWindowRect(false);
		Rect intersect = wndRect;
		if (m_parent != nullptr)
		{
			intersect = wndRect.IntersectRect(&GetClipRect(m_parent));
		}

		if (!intersect.PointInRect(pt))
		{
			return HIT_NOTHING;
		}

		if (GetTitleBarRect(wndRect).PointInRect(pt))
		{
			return HIT_TITLEBAR;
		}
		else if (GetClientRect(false).PointInRect(pt))
		{
			return HIT_CLIENT;
		}

		// Title bar buttons
		if (GetSystemMenuButtonRect(wndRect).PointInRect(pt))
		{
			return HIT_SYSMENU;
		}
		if (GetMinimizeButtonRect(wndRect).PointInRect(pt))
		{
			return HIT_MINBUTTON;
		}
		if (GetMaximizeButtonRect(wndRect).PointInRect(pt))
		{
			return HIT_MAXBUTTON;
		}

		HitZone scrollHit = m_scrollBars->HitTest(pt);
		if (scrollHit != HIT_NOTHING)
			return scrollHit;

		// Resize handles
		if (m_showState & (WindowState::WS_MAXIMIZED | WindowState::WS_MINIMIZED) ||
			!(m_flags & WindowFlags::WIN_CANRESIZE))
		{
			return HIT_NOTHING;
		}

		bool left = pt->x < wndRect.x + 2*m_borderWidth;
		bool top = pt->y < wndRect.y + 2*m_borderWidth;
		bool right = pt->x > wndRect.x + wndRect.w - 2*m_borderWidth;
		bool bottom = pt->y > wndRect.y + wndRect.h - 2*m_borderWidth;
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
		else return left ? HIT_BORDER_LEFT : HIT_BORDER_RIGHT;
	}

	Rect Window::GetClipRect(WindowRef win)
	{
		Rect rect = win->GetClientRect(false);

		while (win)
		{
			rect = rect.IntersectRect(&win->GetClientRect(false));
			win = win->GetParent();
		}

		return rect;
	}

	void Window::Draw()
	{
		if (!(m_showState & WindowState::WS_VISIBLE))
			return;

		if (m_parent != nullptr)
		{
			Rect clip = GetClipRect(m_parent);
			SDL_RenderSetClipRect(m_renderer, &clip);
		}

		Rect rect = GetWindowRect(false);

		bool active = (WINMGR().GetActive() == this || (m_flags & WindowFlags::WIN_ACTIVE));
		DrawReliefBox(&rect, Color::C_LIGHT_GREY, false);
		DrawTitleBar(rect, active);
		DrawTitle(rect, active);

		if (m_flags & WindowFlags::WIN_SYSMENU)
		{
			DrawSystemMenuButton(rect, Color::C_LIGHT_GREY);
		}
		if (m_flags & WindowFlags::WIN_MINMAX)
		{
			DrawMinMaxButtons(rect, Color::C_LIGHT_GREY);
		}

		if (!(m_showState & WS_MINIMIZED))
		{
			m_scrollBars->Draw();
		}

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

	void Window::ToggleButtonState(HitZone button, bool pushed)
	{
		if (pushed)
		{
			m_pushedState = HitZone(m_pushedState | button);
		}
		else
		{
			m_pushedState = HitZone(m_pushedState & ~button);
		}
	}

	void Window::ButtonPushed(HitZone button)
	{
		const ScrollStateRef scroll = m_scrollBars->GetScrollState();

		//TODO: Improve this
		int hScroll = scroll->hMax / 10;
		int vScroll = scroll->vMax / 10;

		switch (button)
		{
		case HIT_MAXBUTTON: Maximize(); break;
		case HIT_MINBUTTON: Minimize(); break;
		case HIT_SYSMENU: break;
		case HIT_HSCROLL_LEFT: m_scrollBars->ScrollRel(&Point({ -hScroll, 0 })); break;
		case HIT_HSCROLL_RIGHT: m_scrollBars->ScrollRel(&Point({ hScroll, 0 })); break;
		case HIT_VSCROLL_UP: m_scrollBars->ScrollRel(&Point({ 0, -vScroll })); break;
		case HIT_VSCROLL_DOWN: m_scrollBars->ScrollRel(&Point({ 0, vScroll })); break;
		}
	}

	bool Window::MoveRel(PointRef rel)
	{
		bool clip = false;
		if ((m_flags & WindowFlags::WIN_CANMOVE) &&
			!(m_showState & WindowState::WS_MAXIMIZED))
		{
			m_rect.x += rel->x;
			m_rect.y += rel->y;

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

	bool Window::ResizeRel(PointRef rel)
	{
		bool clip = false;
		if (m_flags & WindowFlags::WIN_CANRESIZE &&
			!(m_showState & WindowState::WS_MAXIMIZED))
		{
			m_rect.w += rel->x;
			m_rect.h += rel->y;

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

	void Window::Minimize()
	{
		if (m_showState & WindowState::WS_MINIMIZED)
		{
			Restore();
			m_parent->SetMinimizedChild(this, false);
		}
		else
		{
			m_showState = WindowState(m_showState | WindowState::WS_MINIMIZED);
			m_showState = WindowState(m_showState & ~WindowState::WS_MAXIMIZED);

			m_parent->SetMinimizedChild(this, true);
		}
	}

	void Window::Maximize()
	{
		if (m_showState & WindowState::WS_MAXIMIZED)
		{
			Restore();
		}
		else
		{
			m_scrollBars->ScrollTo(&Point({ 0,0 }));
			m_showState = WindowState(m_showState | WindowState::WS_MAXIMIZED);
			m_showState = WindowState(m_showState & ~WindowState::WS_MINIMIZED);
		}
	}

	void Window::Restore()
	{
		m_showState = WindowState(m_showState & ~(WindowState::WS_MAXIMIZED | WindowState::WS_MINIMIZED));
	}

	int Window::GetMinimizedChildIndex(WindowRef child) const
	{
		auto find = std::find_if(m_minimizedChildren.begin(), m_minimizedChildren.end(), [child](WindowRef min) { return min == child; });
		if (find != m_minimizedChildren.end())
		{
			return (int)(find - m_minimizedChildren.begin());
		}

		return -1;
	}

	void Window::SetMinimizedChild(WindowRef child, bool add)
	{
		if (add)
		{
			if (GetMinimizedChildIndex(child) != -1)
				return;

			auto firstHole = std::find_if(m_minimizedChildren.begin(), m_minimizedChildren.end(), [](WindowRef min) { return min == nullptr; });
			if (firstHole == m_minimizedChildren.end())
			{
				m_minimizedChildren.push_back(child);
			}
			else
			{
				*firstHole = child;
			}
		}
		else
		{
			auto find = std::find_if(m_minimizedChildren.begin(), m_minimizedChildren.end(), [child](WindowRef min) { return min == child; });
			if (find != m_minimizedChildren.end())
			{
				*find = nullptr;
			}
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