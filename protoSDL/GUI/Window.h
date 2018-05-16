#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Color.h"
#include "WindowManager.h"
#include "ScrollBars.h"
#include <string>
#include <vector>

namespace GUI
{
	class Window : public Widget
	{
	public:
		using MinWindowList = std::vector<WindowRef>;	

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

		HitZone HitTest(const PointRef) override;
		void Draw() override;

		WindowState GetShowState() const { return m_showState; }

		bool HasParent() const { return m_parent != nullptr; }
		WindowRef GetParent() const { return m_parent; }

		WindowFlags GetFlags() { return m_flags; }
		void ToggleButtonState(HitZone button, bool pushed);
		void ButtonPushed(HitZone button);

		bool MoveRel(PointRef rel);
		bool ResizeRel(PointRef rel);

		void Maximize();
		void Minimize();
		void Restore();

		PointRef GetScroll() { return m_scrollBars->GetScrollPos(); }
		ScrollBarsRef GetScrollBars() { return m_scrollBars.get(); }

		bool GetPushedState(HitZone id) { return m_pushedState & id; }

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

		void DrawMinMaxButtons(Rect pos, const GUI::Color & col);
		void DrawSystemMenuButton(Rect pos, const GUI::Color & col);
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

		std::string m_id;
		GUI::WindowRef m_parent;
		FontRef m_font;
		static ImagePtr m_titleBackground;
		TexturePtr m_activeTitle;
		TexturePtr m_inactiveTitle;
		ImageRef m_image;
		MinWindowList m_minimizedChildren;
		ScrollBarsPtr m_scrollBars;

		static Window m_nullWnd;		

		struct shared_enabler;

		friend class ScrollBars;
	};
}