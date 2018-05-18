#include "SDL.h"
#include "ResourceManager.h"
#include "WindowManager.h"
#include "Window.h"
#include <algorithm>
#include <iostream>
#include "Basetsd.h"

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

	HitResult WindowManager::HitTest(PointRef pt)
	{
		for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
		{
			auto & window = *it;
			auto hitResult = window->HitTest(pt);

			if (hitResult)
			{
				return hitResult;
			}
		}

		return HIT_NOTHING;
	}

	GUI::WindowRef WindowManager::GetActive()
	{
		return m_activeWindow ? m_activeWindow : Window::GetNullWnd();
	}

	void WindowManager::SetActive(WindowRef win)
	{
		m_activeWindow = win;
		MoveToFront(win);
	}
	const CaptureInfo & WindowManager::StartCapture(HitResult hit, PointRef pt)
	{
		if (hit && hit.target)
		{
			m_capture.Captured = true;
			m_capture.Target = hit;
			m_capture.Origin = hit.target->GetRect(true, false);
			m_capture.Delta = m_capture.Origin.Origin();
			m_capture.Delta.x -= pt->x;
			m_capture.Delta.y -= pt->y;
		}
		return m_capture;
	}

	Uint32 timerCallbackFunc(Uint32 interval, void *param)
	{
		SDL_Event event;
		SDL_UserEvent userevent;

		userevent.type = SDL_USEREVENT;
		userevent.code = PtrToUlong(param);
		userevent.data1 = NULL;
		userevent.data2 = NULL;

		event.type = SDL_USEREVENT;
		event.user = userevent;

		SDL_PushEvent(&event);
		return(interval);
	}

	Uint32 WindowManager::AddTimer(Uint32 interval)
	{
		Uint32 eventID = SDL_RegisterEvents(1);
		if (eventID == ((Uint32)-1))
		{
			throw std::exception("Unable to allocate timer");
		}
		
		SDL_TimerID timerID = SDL_AddTimer(interval, timerCallbackFunc, LongToPtr(eventID));
		m_timers[eventID] = timerID;
		return eventID;
	}

	void WindowManager::DeleteTimer(Uint32 timerID)
	{
		const auto & it = m_timers.find(timerID);
		if (it == m_timers.end())
		{
			throw std::invalid_argument("timer not found");
		}

		if (SDL_RemoveTimer(it->second))
		{
			m_timers.erase(timerID);
		}
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

	void WindowManager::MoveToFront(WindowRef win)
	{
		if (win == nullptr)
			return;

		if (win->HasParent())
		{
			RaiseChildren(win->GetParentWnd());
		}
		RaiseChildren(win);
	}
}