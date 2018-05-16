#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Color.h"
#include "WindowManager.h"
#include <string>
#include <vector>

namespace GUI
{
	struct ScrollState
	{
		ScrollState() : showH(false), showV(false), hMax(0), vMax(0) {}
		bool showH, showV;
		Point scrollPos;
		int hMax, vMax;

		Rect leftButton, rightButton;
		Rect upButton, downButton;
		Rect hSlider, vSlider;
		Rect hScrollArea, vScrollArea;
	};

	class ScrollBars : public Widget
	{
	public:
		virtual ~ScrollBars() = default;
		ScrollBars(const ScrollBars&) = delete;
		ScrollBars& operator=(const ScrollBars&) = delete;
		ScrollBars(ScrollBars&&) = delete;
		ScrollBars& operator=(ScrollBars&&) = delete;

		static ScrollBarsPtr Create(RendererRef renderer, WindowRef parent);

		HitZone HitTest(const PointRef) override;
		void Draw() override;

		void ScrollRel(PointRef pt);
		void ScrollTo(PointRef pt);
		void ClickHScrollBar(PointRef pt);
		void ClickVScrollBar(PointRef pt);

		PointRef GetScrollPos() { return &m_scrollState.scrollPos; }
		ScrollStateRef GetScrollState() { return &m_scrollState; }
		uint8_t GetSize() const { return m_scrollBarSize; }

	protected:
		ScrollBars(RendererRef renderer, WindowRef parent);

		void DrawScrollBars(RectRef pos);
		void DrawHScrollBar(RectRef pos);
		void DrawVScrollBar(RectRef pos);

		static uint8_t constexpr m_scrollBarSize = 17;

		WindowRef m_parent;
		ScrollState m_scrollState;

		struct shared_enabler;
	};
}