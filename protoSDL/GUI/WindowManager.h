#pragma once
#include "Common.h"
#include "Color.h"
#include <string>
#include <list>
#include <functional>

namespace GUI
{
	class WindowManager
	{
	public:
		using WindowList = std::list<GUI::WindowPtr>;

		virtual ~WindowManager() = default;
		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;
		WindowManager(WindowManager&&) = delete;
		WindowManager& operator=(WindowManager&&) = delete;

		static WindowManager & Get();

		void Init(RendererPtr & renderer);

		void Draw();
		GUI::WindowPtr AddWindow(const char* id, SDL_Rect pos);
		GUI::WindowPtr AddWindow(const char* id, GUI::WindowPtr parent, SDL_Rect pos);
		GUI::WindowPtr FindWindow(const char* id);
		WindowList GetWindowList(GUI::WindowRef parent);

		GUI::WindowPtr HitTest(SDL_Point);
		GUI::WindowRef GetActive() { return m_activeWindow.get(); }
		void SetActive(GUI::WindowPtr);

	protected:
		WindowManager() : m_renderer(nullptr) {}
		RendererRef m_renderer;

		WindowList m_windows;
		GUI::WindowPtr m_activeWindow;
	};

	constexpr auto WINMGR = &WindowManager::Get;

}