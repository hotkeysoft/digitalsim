#pragma once
#include "Common.h"
#include "Color.h"
#include "WindowManager.h"
#include <string>

namespace GUI
{
	class Window
	{
	public:

		virtual ~Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		static WindowPtr Create(const char* id, RendererRef renderer, WindowRef parent, FontRef font, SDL_Rect rect, WindowFlags flags);
		void SetTitle(const char* title);
		void SetImage(ImageRef image);

		static WindowRef GetNullWnd() { return &m_nullWnd; }

		std::string GetId() const { return m_id; }
		WindowManager::WindowList GetChildWindows();

		SDL_Rect GetClientRect(bool relative = true) const;
		SDL_Rect GetWindowRect(bool relative = true) const;

		HitZone HitTest(SDL_Point);

		void Draw();

		WindowState GetShowState() const { return m_showState; }

		bool HasParent() const { return m_parent != nullptr; }
		WindowRef GetParent() const { return m_parent; }

		WindowFlags GetFlags() { return m_flags; }
		void ToggleButtonState(HitZone button, bool pushed);
		void ButtonPushed(HitZone button);

		bool MoveRel(SDL_Point rel);
		bool ResizeRel(SDL_Point rel);

		void Maximize();
		void Minimize();
		void Restore();

	protected:
		Window(const char* id, RendererRef renderer, WindowRef parent, FontRef font, SDL_Rect rect, WindowFlags flags);
		Window();

		SDL_Rect GetTitleBarRect(SDL_Rect base) const;
		SDL_Rect GetSystemMenuButtonRect(SDL_Rect base) const;
		SDL_Rect GetMinimizeButtonRect(SDL_Rect base) const;
		SDL_Rect GetMaximizeButtonRect(SDL_Rect base) const;

		void SetDrawColor(const GUI::Color & col);
		void Draw3dFrame(SDL_Rect pos, bool raised);
		void DrawReliefBox(SDL_Rect pos, const GUI::Color & col, bool raised);
		void DrawMinMaxButtons(SDL_Rect pos, const GUI::Color & col);
		void DrawSystemMenuButton(SDL_Rect pos, const GUI::Color & col);
		void DrawButton(SDL_Rect pos, const GUI::Color & col, ImageRef image, bool raised);
		void DrawTitleBar(SDL_Rect rect, bool active);
		void DrawTitle(SDL_Rect rect, bool active);
		void RenderTitle();
		TexturePtr SurfaceToTexture(SDL_Surface* surf);

		std::string m_title;
		WindowFlags m_flags;
		WindowState m_showState;
		HitZone m_pushedState;
		SDL_Rect m_rect;
		SDL_Rect m_titleStrRect;

		static uint8_t constexpr m_borderWidth = 6;
		static uint8_t constexpr m_buttonSize = 24;

		std::string m_id;
		GUI::WindowRef m_parent;
		RendererRef m_renderer;
		FontRef m_font;
		static ImagePtr m_titleBackground;
		TexturePtr m_activeTitle;
		TexturePtr m_inactiveTitle;
		ImageRef m_image;

		static Window m_nullWnd;		

		struct shared_enabler;
	};
}