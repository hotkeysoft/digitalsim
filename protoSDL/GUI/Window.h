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
		using ControlList = std::map<std::string, WidgetPtr>;

		virtual ~Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		static WindowRef GetNullWnd() { return &m_nullWnd; }

		static WindowPtr Create(const char* id, RendererRef renderer, WindowRef parent, FontRef font, Rect rect, WindowFlags flags);
		void SetText(const char * text) override;

		WindowManager::WindowList GetChildWindows();

		void AddControl(WidgetPtr);
		WidgetPtr FindControl(const char* id) const;
		const ControlList &GetControls() const { return m_controls; }

		WindowRef GetParentWnd() const { return static_cast<WindowRef>(m_parent); }

		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;
		Rect GetRect(bool relative = true, bool scrolled = true) const override;

		HitResult HitTest(const PointRef) override;
		void Draw() override;

		WindowState GetShowState() const { return m_showState; }

		WindowFlags GetFlags() { return m_flags; }
		void ToggleButtonState(HitZone button, bool pushed);
		void ButtonPushed(HitZone button);

		bool MoveRel(PointRef rel);
		bool MovePos(PointRef pos);
		bool MoveRect(RectRef rect);
		bool ResizeRel(PointRef rel);
		bool Resize(PointRef size);

		void Maximize();
		void Minimize();
		void Restore();

		ScrollBarsRef GetScrollBars() { return m_scrollBars.get(); }

		bool GetPushedState(HitZone id) { return m_pushedState & id; }

	protected:
		Window(const char* id, RendererRef renderer, WindowRef parent, FontRef font, Rect rect, WindowFlags flags);
		Window();

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
		void DrawControls();
		void RenderTitle();

		Rect GetClipRect(WindowRef win);

		WindowFlags m_flags;
		WindowState m_showState;
		HitZone m_pushedState;

		static ImagePtr m_titleBackground;

		TexturePtr m_activeTitle;
		TexturePtr m_inactiveTitle;
		Rect m_titleStrRect;

		MinWindowList m_minimizedChildren;
		ScrollBarsPtr m_scrollBars;

		static Window m_nullWnd;		

		ControlList m_controls;

		struct shared_enabler;

		friend class ScrollBars;
	};
}