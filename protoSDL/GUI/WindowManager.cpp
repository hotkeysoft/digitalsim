#include "SDL.h"
#include "SDL_rect.h"
#include "ResourceManager.h"
#include "WindowManager.h"
#include "Window.h"
#include <algorithm>
#include <iostream>

namespace GUI
{
	WindowManager & WindowManager::Get()
	{
		static WindowManager manager;
		return manager;
	}

	void WindowManager::Init(RendererPtr & renderer)
	{
		m_renderer = renderer.get();
	}

	void WindowManager::Draw()
	{
		for (auto & window : m_windows)
		{
			if (window->IsVisible())
			{
				window->Draw();
			}
		}
	}

	WindowPtr WindowManager::AddWindow(const char * id, SDL_Rect pos)
	{
		return AddWindow(id, nullptr, pos);
	}

	WindowPtr WindowManager::AddWindow(const char* id, WindowPtr parent, SDL_Rect pos)
	{
		if (FindWindow(id) != nullptr)
		{
			throw std::invalid_argument("windows already exists:" + (std::string)id);
		}

		WindowPtr newWindow = Window::Create(id, m_renderer, parent.get(), RES().FindFont("default"), pos);
		m_windows.push_back(newWindow);

		return newWindow;
	}

	WindowPtr WindowManager::FindWindow(const char* id)
	{
		if (id == nullptr)
		{
			throw std::invalid_argument("id can't be null");
		}

		for (auto & it : m_windows)
		{
			if (id == it->GetId())
			{
				return it;
			}
		}

		return nullptr;
	}

	WindowManager::WindowList WindowManager::GetWindowList(WindowRef parent)
	{
		WindowList childWindows;
		std::copy_if(m_windows.begin(), m_windows.end(), std::back_inserter(childWindows),
			[parent](WindowPtr window) { return window->GetParent() == parent; });
		return childWindows;
	}

	WindowPtr WindowManager::HitTest(SDL_Point pt)
	{
		// print window stack
		for (auto & win : m_windows)
		{
			std::cout << win->GetId() << " ";
		}
		std::cout << std::endl;

		std::cout << "HitTest: ";
		for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
		{
			auto & window = *it;
			std::cout << window->GetId() << " ";
			if (window->IsVisible())
			{
				SDL_Rect rect = window->GetWindowRect(false);
				if (SDL_PointInRect(&pt, &rect))
				{
					std::cout << "HIT" << std::endl;
					return window;
				}
			}
		}

		std::cout << "NO HIT" << std::endl;
		return nullptr;
	}

	GUI::WindowRef WindowManager::GetActive()
	{
		return m_activeWindow ? m_activeWindow.get() : Window::GetNullWnd();
	}

	void WindowManager::SetActive(WindowPtr win)
	{
		std::cout << "SetActive(): " << win->GetId() << std::endl;
		m_activeWindow = win;
		MoveToFront(win);

		// print window stack
		for (auto & win : m_windows)
		{
			std::cout << win->GetId() << " ";
		}
		std::cout << std::endl;
	}
	void WindowManager::RaiseSingleWindow(WindowRef win)
	{
		const std::string & id = win->GetId();

		auto found = std::find_if(m_windows.begin(), m_windows.end(), [id](WindowPtr window) { return window->GetId() == id; });
		if (found != m_windows.end())
		{
			m_windows.splice(m_windows.end(), m_windows, found);
		}
	}

	void WindowManager::MoveToFront(WindowPtr win)
	{
		if (win == nullptr)
			return;

		// Move parent to front
//		RaiseSingleWindow(win->GetParent());

		// Move self
		RaiseSingleWindow(win.get());


		// Move children
		for (auto & child : win->GetChildWindows())
		{
			MoveToFront(child);
		}


	}
}