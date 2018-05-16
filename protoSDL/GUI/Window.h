#pragma once
#include "Common.h"
#include "Rect.h"
#include "Color.h"
#include "WindowManager.h"
#include <string>
#include <vector>

namespace GUI
{
	class Window
	{
	public:
		using MinWindowList = std::vector<WindowRef>;
		
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

		virtual ~Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		static WindowPtr Create(const char* id, RendererRef renderer, WindowRef parent, FontRef font, Rect rect, WindowFlags flags);
		void SetTitle(const char* title);
		void SetImage(ImageRef image);

		static WindowRef GetNullWnd() { return &m_nullWnd; }

		std::string GetId() const { return m_id; }
		WindowManager::WindowList GetChildWindows();

		Rect GetClientRect(bool relative = true) const;
		Rect GetWindowRect(bool relative = true, bool scrolled = true) const;

		HitZone HitTest(PointRef);

		void Draw();

		WindowState GetShowState() const { return m_showState; }

		bool HasParent() const { return m_parent != nullptr; }
		WindowRef GetParent() const { return m_parent; }

		WindowFlags GetFlags() { return m_flags; }
		void ToggleButtonState(HitZone button, bool pushed);
		void ButtonPushed(HitZone button);

		bool MoveRel(PointRef rel);
		bool ResizeRel(PointRef rel);
		void ScrollRel(PointRef pt);
		void ScrollTo(PointRef pt);
		void ClickHScrollBar(PointRef pt);
		void ClickVScrollBar(PointRef pt);

		void Maximize();
		void Minimize();
		void Restore();

		PointRef GetScroll() { return &m_scrollState.scrollPos; }

	protected:
		Window(const char* id, RendererRef renderer, WindowRef parent, FontRef font, Rect rect, WindowFlags flags);
		Window();

		ScrollState GetScrollBarState() const;

		Rect GetTitleBarRect(Rect base) const;
		Rect GetSystemMenuButtonRect(Rect base) const;
		Rect GetMinimizeButtonRect(Rect base) const;
		Rect GetMaximizeButtonRect(Rect base) const;

		void SetMinimizedChild(WindowRef child, bool add);
		int GetMinimizedChildIndex(WindowRef child) const;

		void SetDrawColor(const GUI::Color & col);
		void Draw3dFrame(Rect pos, bool raised);
		void DrawReliefBox(Rect pos, const GUI::Color & col, bool raised);
		void DrawMinMaxButtons(Rect pos, const GUI::Color & col);
		void DrawSystemMenuButton(Rect pos, const GUI::Color & col);
		void DrawButton(Rect pos, const GUI::Color & col, ImageRef image, bool raised);
		void DrawScrollBars(Rect pos);
		void DrawHScrollBar(Rect pos);
		void DrawVScrollBar(Rect pos);
		void DrawTitleBar(Rect rect, bool active);
		void DrawTitle(Rect rect, bool active);
		void RenderTitle();
		TexturePtr SurfaceToTexture(SDL_Surface* surf);
		Rect GetClipRect(WindowRef win);

		std::string m_title;
		WindowFlags m_flags;
		WindowState m_showState;
		HitZone m_pushedState;
		Rect m_rect;
		Rect m_titleStrRect;

		static uint8_t constexpr m_borderWidth = 4;
		static uint8_t constexpr m_buttonSize = 24;
		static uint8_t constexpr m_scrollBarSize = 17;

		std::string m_id;
		GUI::WindowRef m_parent;
		RendererRef m_renderer;
		FontRef m_font;
		static ImagePtr m_titleBackground;
		TexturePtr m_activeTitle;
		TexturePtr m_inactiveTitle;
		ImageRef m_image;
		MinWindowList m_minimizedChildren;
		ScrollState m_scrollState;

		static Window m_nullWnd;		

		struct shared_enabler;
	};
}