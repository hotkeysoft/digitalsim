#pragma once
#include "Common.h"
#include "Color.h"
#include "Point.h"
#include <string>
#include <ostream>

namespace GUI
{
	class Widget
	{
	public:
		Widget(RendererRef renderer) : m_renderer(renderer) {}

		virtual ~Widget() = default;
		Widget(const Widget&) = delete;
		Widget& operator=(const Widget&) = delete;
		Widget(Widget&&) = delete;
		Widget& operator=(Widget&&) = delete;

		virtual HitZone HitTest(const PointRef) { return HitZone::HIT_NOTHING; }

		virtual void Draw() = 0;

		std::string ToString() const { return ""; }

	protected:
		void SetDrawColor(const GUI::Color & col);
		void DrawButton(const RectRef pos, const GUI::Color & col, ImageRef image, bool raised);
		void Draw3dFrame(const RectRef pos, bool raised);
		void DrawReliefBox(const RectRef pos, const GUI::Color & col, bool raised);

		RendererRef m_renderer;

		static uint8_t constexpr m_borderWidth = 4;
		static uint8_t constexpr m_buttonSize = 24;
	};

	inline std::ostream & operator << (std::ostream & os, const Widget& widget)
	{
		os << widget.ToString();
		return os;
	}
}