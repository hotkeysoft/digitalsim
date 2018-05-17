#pragma once
#include "Common.h"
#include "Color.h"
#include "Point.h"
#include "Rect.h"
#include "ResourceManager.h"
#include <string>
#include <ostream>

namespace GUI
{
	class Widget
	{
	public:
		virtual ~Widget() = default;
		Widget(const Widget&) = delete;
		Widget& operator=(const Widget&) = delete;
		Widget(Widget&&) = delete;
		Widget& operator=(Widget&&) = delete;

		virtual const std::string &GetId() const { return m_id; }

		virtual void Init() {};

		virtual std::string GetText() const { return m_text; }
		virtual void SetText(const char *text) { m_text = text ? text : ""; }

		virtual ImageRef GetImage() const { return m_image; }
		virtual void SetImage(ImageRef image) { m_image = image;  }

		virtual Rect GetClientRect(bool relative = true, bool scrolled = true) const { return m_rect; }
		virtual Rect GetRect(bool relative = true, bool scrolled = true) const { return m_rect; }
		virtual void SetRect(RectRef rect) { m_rect = rect?(*rect):Rect(); }

		// Margins & Padding
		virtual uint8_t GetMargin() { return m_margin; }
		virtual void SetMargin(uint8_t margin) { m_margin = margin; }

		virtual uint8_t GetPadding() { return m_padding; }
		virtual void SetPadding(uint8_t padding) { m_padding = padding; }

		// Font
		virtual const FontRef GetFont() const { return m_font; }
		virtual void SetFont(FontRef font);

		// Borders
		virtual void SetBorder(bool show) { m_showBorder = show; }
		virtual bool GetBorder() { return m_showBorder; }
		
		virtual Color GetBorderColor() { return m_borderColor; }
		virtual void SetBorderColor(Color color) { m_borderColor = std::move(color); }

		virtual uint8_t GetBorderWidth() { return m_borderWidth; }
		virtual void SetBorderWidth(uint8_t width) { m_borderWidth = width; }

		// Colors
		virtual Color GetForegroundColor() { return m_foregroundColor; }
		virtual void SetForegroundColor(Color color) { m_foregroundColor = std::move(color); }

		virtual Color GetBackgroundColor() { return m_backgroundColor; }
		virtual void SetBackgroundColor(Color color) { m_backgroundColor = std::move(color); }

		virtual bool HasParent() const { return m_parent != nullptr; }
		virtual const WidgetRef GetParent() const { return m_parent; }
		virtual void SetParent(WidgetRef parent) { m_parent = parent; }

		virtual HitZone HitTest(const PointRef) { return HitZone::HIT_NOTHING; }

		virtual PointRef GetScrollPos() { return &m_scrollPos; }

		virtual void Draw() = 0;

		virtual std::string ToString() const { return ""; }

	protected:
		Widget(const char* id, RendererRef renderer, WidgetRef parent, Rect rect,
			const char* text, ImageRef image = nullptr, FontRef font = nullptr);
		Widget(const char* id);

		void SetDrawColor(const GUI::Color & col);
		void DrawFilledRect(const RectRef pos, const GUI::Color & col);
		void DrawRect(const RectRef pos, const GUI::Color & col);
		void DrawButton(const RectRef pos, const GUI::Color & col, ImageRef image, bool raised);
		void Draw3dFrame(const RectRef pos, bool raised);
		void DrawReliefBox(const RectRef pos, const GUI::Color & col, bool raised);

		TexturePtr SurfaceToTexture(SDL_Surface* surf);

		std::string m_id;
		RendererRef m_renderer;
		std::string m_text;
		ImageRef m_image;
		WidgetRef m_parent;
		Rect m_rect;
		FontRef m_font;
		Point m_scrollPos;

		// Borders
		bool m_showBorder;
		Color m_borderColor;

		// Colors
		Color m_backgroundColor;
		Color m_foregroundColor;

		uint8_t m_padding;
		uint8_t m_margin;
		uint8_t m_borderWidth;

		static uint8_t constexpr m_buttonSize = 24;
	};

	inline std::ostream & operator << (std::ostream & os, const Widget& widget)
	{
		os << widget.ToString();
		return os;
	}
}