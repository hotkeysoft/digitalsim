#include "SDL.h"
#include "SDL_rect.h"
#include "ResourceManager.h"
#include "WindowManager.h"
#include "Window.h"

namespace GUI
{
	SDL::RendererRef WindowManager::m_renderer = nullptr;
	WindowManager::WindowList WindowManager::m_windows;
	GUI::WindowPtr WindowManager::m_activeWindow = nullptr;

	WindowManager & WindowManager::Get()
	{
		static WindowManager manager;
		return manager;
	}

	void WindowManager::Init(SDL::RendererPtr & renderer)
	{
		m_renderer = renderer.get();
	}

	void WindowManager::Draw()
	{
		for (auto & it : m_windows)
		{
			WindowPtr &window = it.second;
			if (window->IsVisible())
			{
				window->Draw(window == m_activeWindow);
			}
		}
	}

	WindowPtr WindowManager::AddWindow(const char * id, SDL_Rect pos)
	{
		if (FindWindow(id) != nullptr)
		{
			throw std::invalid_argument("windows already exists:" + (std::string)id);
		}

		WindowPtr newWindow = Window::Create(m_renderer, GUI::ResourceManager::Get().FindFont("default"), pos);
		m_windows[id] = newWindow;

		return newWindow;
	}

	WindowPtr WindowManager::FindWindow(const char* id)
	{
		if (id == nullptr)
		{
			throw std::invalid_argument("id can't be null");
		}

		auto it = m_windows.find(id);
		if (it != m_windows.end())
		{
			return it->second;
		}

		return nullptr;
	}

	GUI::WindowPtr WindowManager::HitTest(SDL_Point pt)
	{
		for (auto & it : m_windows)
		{
			WindowPtr &window = it.second;
			if (window->IsVisible())
			{
				SDL_Rect rect = window->GetWindowRect();
				if (SDL_PointInRect(&pt, &rect))
				{
					return window;
				}
			}
		}

		return nullptr;
	}

	void WindowManager::SetActive(GUI::WindowPtr win)
	{
		m_activeWindow = win;
	}

}