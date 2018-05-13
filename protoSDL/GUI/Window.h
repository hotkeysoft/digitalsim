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

		static WindowPtr Create(const char* id, RendererRef renderer, WindowRef parent, FontRef font, SDL_Rect rect, WindowCreationFlags flags);
		void SetTitle(const char* title);
		void SetImage(ImageRef image);

		static WindowRef GetNullWnd() { return &m_nullWnd; }

		std::string GetId() const { return m_id; }
		WindowManager::WindowList GetChildWindows();

		SDL_Rect GetClientRect(bool relative = true) const;
		SDL_Rect GetWindowRect(bool relative = true) const;

		HitZone HitTest(SDL_Point);

		void Draw();

		void SetVisible(bool visible = true) { m_visible = visible; }
		bool IsVisible() const { return m_visible;  }

		bool HasParent() const { return m_parent != nullptr; }
		WindowRef GetParent() const { return m_parent; }

		WindowCreationFlags GetFlags() { return m_flags; }
		bool MoveRel(SDL_Point rel);
		bool ResizeRel(SDL_Point rel);

	protected:
		Window(const char* id, RendererRef renderer, WindowRef parent, FontRef font, SDL_Rect rect, WindowCreationFlags flags);
		Window();

		SDL_Rect GetTitleBarRect() const;
		void SetDrawColor(const GUI::Color & col);
		void Draw3dFrame(SDL_Rect pos, bool raised);
		void DrawReliefBox(SDL_Rect pos, const GUI::Color & col, bool raised);
		void DrawButton(SDL_Rect pos, const GUI::Color & col, bool raised);
		void DrawTitleBar(bool active);
		void DrawTitle(bool active);
		void RenderTitle();
		TexturePtr SurfaceToTexture(SDL_Surface* surf);

		std::string m_title;
		WindowCreationFlags m_flags;
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

		bool m_visible;

		struct shared_enabler;
	};
}