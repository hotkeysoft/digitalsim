#include "SDL.h"
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
			window->Draw();
		}
	}

	WindowPtr WindowManager::AddWindow(const char * id, Rect pos, WindowFlags flags)
	{
		return AddWindow(id, nullptr, pos, flags);
	}

	WindowPtr WindowManager::AddWindow(const char* id, WindowPtr parent, Rect pos, WindowFlags flags)
	{
		if (FindWindow(id) != nullptr)
		{
			throw std::invalid_argument("windows already exists:" + (std::string)id);
		}

		WindowPtr newWindow = Window::Create(id, m_renderer, parent.get(), RES().FindFont("default"), pos, flags);
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

	WindowPtr WindowManager::HitTest(PointRef pt)
	{
		for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
		{
			auto & window = *it;
			if (window->HitTest(pt) != HIT_NOTHING)
			{
				return window;
			}
		}

		return nullptr;
	}

	GUI::WindowRef WindowManager::GetActive()
	{
		return m_activeWindow ? m_activeWindow.get() : Window::GetNullWnd();
	}

	void WindowManager::SetActive(WindowPtr win)
	{
		m_activeWindow = win;
		MoveToFront(win);
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

	void WindowManager::RaiseChildren(WindowRef win)
	{
		RaiseSingleWindow(win);
		for (auto & child : win->GetChildWindows())
		{
			RaiseChildren(child.get());
		}
	}

	void WindowManager::MoveToFront(WindowPtr win)
	{
		if (win == nullptr)
			return;

		if (win->HasParent())
		{
			RaiseChildren(win->GetParentWnd());
		}
		RaiseChildren(win.get());
	}
}