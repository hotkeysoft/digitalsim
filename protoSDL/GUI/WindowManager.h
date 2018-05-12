#pragma once
#include "Common.h"
#include "Color.h"
#include <string>
#include <map>

namespace GUI
{
	class WindowManager
	{
	public:
		using WindowList = std::map<std::string, GUI::WindowPtr>;

		virtual ~WindowManager() = default;
		WindowManager(const WindowManager&) = delete;
		WindowManager& operator=(const WindowManager&) = delete;
		WindowManager(WindowManager&&) = delete;
		WindowManager& operator=(WindowManager&&) = delete;

		static WindowManager & Get();
		static void Init(SDL::RendererPtr & renderer);

		static void Draw();
		static GUI::WindowPtr AddWindow(const char* id, SDL_Rect pos);
		static GUI::WindowPtr FindWindow(const char* id);

		static GUI::WindowPtr HitTest(SDL_Point);
		static void SetActive(GUI::WindowPtr);

	protected:
		WindowManager() = default;
		static SDL::RendererRef m_renderer;

		static WindowList m_windows;
		static GUI::WindowPtr m_activeWindow;
	};
}