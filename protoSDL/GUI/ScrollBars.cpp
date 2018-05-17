#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "ScrollBars.h"
#include <algorithm>

namespace GUI
{
	ScrollBars::ScrollBars(RendererRef renderer, WindowRef parent) :
		Widget("scroll", renderer, nullptr, Rect(), nullptr), m_parent(parent)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
	}

	struct ScrollBars::shared_enabler : public ScrollBars
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: ScrollBars(std::forward<Args>(args)...)
		{
		}
	};

	ScrollBarsPtr ScrollBars::Create(RendererRef renderer, WindowRef parent)
	{
		return std::make_unique<shared_enabler>(renderer, parent);
	}

	void ScrollBars::DrawHScrollBar(RectRef pos)
	{		
		DrawButton(pos, Color::C_MED_GREY, nullptr, false);

		int scrollAreaWidth = pos->w - (2 * m_scrollBarSize);
		m_scrollState.leftButton = { pos->x, pos->y, m_scrollBarSize, m_scrollBarSize };
		m_scrollState.rightButton = { pos->x + pos->w - m_scrollBarSize, pos->y, m_scrollBarSize, m_scrollBarSize };
		m_scrollState.hScrollArea = { pos->x + m_scrollBarSize, pos->y, scrollAreaWidth, m_scrollBarSize };

		DrawButton(&m_scrollState.leftButton, Color::C_LIGHT_GREY, RES().FindImage("win.scroll.left"), !m_parent->GetPushedState(HIT_HSCROLL_LEFT));
		DrawButton(&m_scrollState.rightButton, Color::C_LIGHT_GREY, RES().FindImage("win.scroll.right"), !m_parent->GetPushedState(HIT_HSCROLL_RIGHT));

		double sliderHeight = (1.0 - ((double)m_scrollState.hMax / (double)pos->w)) * (double)scrollAreaWidth + 1;
		if (sliderHeight < (m_borderWidth * 2))
		{
			sliderHeight = m_borderWidth * 2;
		}

		int currPos = m_parent->m_scrollPos.x * scrollAreaWidth / pos->w;
		if (currPos + sliderHeight > scrollAreaWidth)
		{
			currPos = scrollAreaWidth - (int)sliderHeight + 1;
		}

		m_scrollState.hSlider = { pos->x + m_scrollBarSize + currPos, pos->y, (int)sliderHeight, m_scrollBarSize };

		DrawButton(&m_scrollState.hSlider, Color::C_LIGHT_GREY, nullptr, !m_parent->GetPushedState(HIT_HSCROLL_SLIDER));
	}

	void ScrollBars::DrawVScrollBar(RectRef pos)
	{
		DrawButton(pos, Color::C_MED_GREY, nullptr, false);

		int scrollAreaHeight = pos->h - (2 * m_scrollBarSize);
		m_scrollState.upButton = { pos->x, pos->y, m_scrollBarSize, m_scrollBarSize };
		m_scrollState.downButton = { pos->x, pos->y + pos->h - m_scrollBarSize, m_scrollBarSize, m_scrollBarSize };
		m_scrollState.vScrollArea = { pos->x, pos->y + m_scrollBarSize, m_scrollBarSize, scrollAreaHeight };

		DrawButton(&m_scrollState.upButton, Color::C_LIGHT_GREY, RES().FindImage("win.scroll.up"), !m_parent->GetPushedState(HIT_VSCROLL_UP));
		DrawButton(&m_scrollState.downButton, Color::C_LIGHT_GREY, RES().FindImage("win.scroll.down"), !m_parent->GetPushedState(HIT_VSCROLL_DOWN));
		
		double sliderHeight = (1.0 - ((double)m_scrollState.vMax / (double)pos->h)) * (double)scrollAreaHeight + 1;
		if (sliderHeight < (m_borderWidth * 2))
		{
			sliderHeight = m_borderWidth * 2;
		}

		int currPos = m_parent->m_scrollPos.y * scrollAreaHeight / pos->h;
		if (currPos + sliderHeight > scrollAreaHeight)
		{
			currPos = scrollAreaHeight - (int)sliderHeight + 1;
		}

		m_scrollState.vSlider = { pos->x, pos->y + m_scrollBarSize + currPos, m_scrollBarSize, (int)sliderHeight };

		DrawButton(&m_scrollState.vSlider, Color::C_LIGHT_GREY, nullptr, !m_parent->GetPushedState(HIT_VSCROLL_SLIDER));
	}

	void ScrollBars::CheckChildScrollStatus(WidgetRef child, RectRef thisRect, bool &showH, bool &showV)
	{
		Rect parentRect = child->GetParent()->GetClientRect(true);

		if (((thisRect->x + thisRect->w) > parentRect.w))
		{
			showH = true;
			int hMax = ((thisRect->x + thisRect->w) - parentRect.w);
			m_scrollState.hMax = std::max(m_scrollState.hMax, hMax);
		}

		if ((thisRect->y + thisRect->h) > parentRect.h)
		{
			showV = true;
			int vMax = ((thisRect->y + thisRect->h) - parentRect.h);
			m_scrollState.vMax = std::max(m_scrollState.vMax, vMax);
		}
	}

	void ScrollBars::DrawScrollBars(RectRef pos)
	{
		if (m_parent->GetFlags() & WindowFlags::WIN_NOSCROLL)
			return;

		m_scrollState.hMax = 0;
		m_scrollState.vMax = 0;

		bool showH = false;
		bool showV = false;
		for (auto & child : m_parent->GetChildWindows())
		{
			if (child->GetParent() != nullptr && !(child->GetShowState() & (WindowState::WS_MAXIMIZED | WindowState::WS_MINIMIZED)))
			{
				Rect childRect = child->GetRect(true, false);
				CheckChildScrollStatus(child.get(), &childRect, showH, showV);
			}
		}
		for (auto & child : m_parent->GetControls())
		{
			CheckChildScrollStatus(child.second.get(), &child.second->GetRect(true, false), showH, showV);
		}
		
		m_scrollState.showH = showH || m_parent->m_scrollPos.x;
		m_scrollState.showV = showV || m_parent->m_scrollPos.y;

		if (m_scrollState.showH)
		{
			Rect hPos = *pos;
			hPos.x += m_borderWidth + 1;
			hPos.w -= (2*m_borderWidth) + 2 + (m_scrollState.showV ? m_scrollBarSize : 0);

			hPos.y += hPos.h - (m_scrollBarSize + m_borderWidth + 1);
			hPos.h = m_scrollBarSize;

			DrawHScrollBar(&hPos);
		}

		if (m_scrollState.showV)
		{
			Rect hPos = *pos;
			hPos.x += hPos.w - (m_scrollBarSize + m_borderWidth + 1);
			hPos.w = m_scrollBarSize;

			hPos.y += m_borderWidth + m_buttonSize + 2 + 1;
			hPos.h -= (2 * m_borderWidth) + m_buttonSize + 2 + 2 + (m_scrollState.showH ? m_scrollBarSize : 0);

			DrawVScrollBar(&hPos);
		}

		if (m_scrollState.showH && m_scrollState.showV)
		{
			Rect hPos = *pos;
			hPos.x += hPos.w - (m_scrollBarSize + m_borderWidth + 1);
			hPos.y += hPos.h - (m_scrollBarSize + m_borderWidth + 1);
			hPos.w = m_scrollBarSize;
			hPos.h = m_scrollBarSize;

			Draw3dFrame(&hPos, true);
		}
	}

	HitZone ScrollBars::HitTest(const PointRef pt)
	{
		if (m_scrollState.showH)
		{
			if (m_scrollState.leftButton.PointInRect(pt))
			{
				return HIT_HSCROLL_LEFT;
			}
			if (m_scrollState.rightButton.PointInRect(pt))
			{
				return HIT_HSCROLL_RIGHT;
			}
			if (m_scrollState.hSlider.PointInRect(pt))
			{
				return HIT_HSCROLL_SLIDER;
			}
			if (m_scrollState.hScrollArea.PointInRect(pt))
			{
				return HIT_HSCROLL_AREA;
			}
		}
		if (m_scrollState.showV)
		{
			if (m_scrollState.upButton.PointInRect(pt))
			{
				return HIT_VSCROLL_UP;
			}
			if (m_scrollState.downButton.PointInRect(pt))
			{
				return HIT_VSCROLL_DOWN;
			}
			if (m_scrollState.vSlider.PointInRect(pt))
			{
				return HIT_VSCROLL_SLIDER;
			}
			if (m_scrollState.vScrollArea.PointInRect(pt))
			{
				return HIT_VSCROLL_AREA;
			}
		}
		return HIT_NOTHING;
	}

	void ScrollBars::Draw()
	{
		Rect rect = m_parent->GetRect(false);

		if (!(m_parent->GetShowState() & WS_MINIMIZED))
		{
			DrawScrollBars(&rect);
		}
	}

	void ScrollBars::ScrollRel(PointRef pt)
	{ 
		if (m_parent->GetFlags() & WindowFlags::WIN_NOSCROLL)
			return;

		if (m_scrollState.showH)
		{
			m_parent->m_scrollPos.x += pt->x;
			m_parent->m_scrollPos.x = clip(m_parent->m_scrollPos.x, 0, m_scrollState.hMax);
		}

		if (m_scrollState.showV)
		{
			m_parent->m_scrollPos.y += pt->y;
			m_parent->m_scrollPos.y = clip(m_parent->m_scrollPos.y, 0, m_scrollState.vMax);
		}
	}

	void ScrollBars::ScrollTo(PointRef pt)
	{
		if (m_parent->GetFlags() & WindowFlags::WIN_NOSCROLL)
			return;

		m_parent->m_scrollPos.x = clip(pt->x, 0, m_scrollState.hMax);
		m_parent->m_scrollPos.y = clip(pt->y, 0, m_scrollState.vMax);
	}

	void ScrollBars::ClickHScrollBar(PointRef pt)
	{
		int rel = (pt->x - m_scrollState.hScrollArea.x) * m_scrollState.hMax / m_scrollState.hScrollArea.w;
		m_parent->m_scrollPos.x = clip(rel, 0, m_scrollState.hMax);
	}

	void ScrollBars::ClickVScrollBar(PointRef pt)
	{
		int rel = (pt->y - m_scrollState.vScrollArea.y) * m_scrollState.vMax / m_scrollState.vScrollArea.h;
		m_parent->m_scrollPos.y = clip(rel, 0, m_scrollState.vMax);
	}
}