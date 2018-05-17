#pragma once
#include "Common.h"
#include "Color.h"
#include "Rect.h"
#include "Point.h"
#include <string>
#include <list>
#include <functional>

namespace GUI
{
	class WindowManager
	{
	public:
		using WindowList = std::list<WindowPtr>;

		virtual ~WindowManager() = default;
		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;
		WindowManager(WindowManager&&) = delete;
		WindowManager& operator=(WindowManager&&) = delete;

		static WindowManager & Get();

		void Init(RendererPtr & renderer);

		void Draw();
		WindowPtr AddWindow(const char* id, Rect pos, WindowFlags flags = WindowFlags::WIN_DEFAULT);
		WindowPtr AddWindow(const char* id, WindowPtr parent, Rect pos, WindowFlags flags = WindowFlags::WIN_DEFAULT);
		WindowPtr FindWindow(const char* id);
		WindowList GetWindowList(WindowRef parent);

		void MoveToFront(WindowRef);

		HitResult HitTest(PointRef);
		WindowRef GetActive();
		void SetActive(WindowRef);

	protected:
		void RaiseSingleWindow(WindowRef);
		void RaiseChildren(WindowRef);

		WindowManager() : m_renderer(nullptr) {}
		RendererRef m_renderer;

		WindowList m_windows;
		WindowRef m_activeWindow;
	};

	constexpr auto WINMGR = &WindowManager::Get;

}