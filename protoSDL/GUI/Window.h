#pragma once
#include "Common.h"
#include "Color.h"
#include <string>

namespace GUI
{
	class Window
	{
	public:
		enum HitZone {
			HIT_NOTHING = 0,
			HIT_TITLEBAR = 1,
			HIT_CLIENT = 2,
			HIT_BORDER_CORNER = 4,
			HIT_BORDER_H = 8,
			HIT_BORDER_V = 16,
			HIT_BORDER_ANY = HIT_BORDER_CORNER | HIT_BORDER_H | HIT_BORDER_V
		};

		virtual ~Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		static WindowPtr Create(SDL::RendererRef renderer, SDL::FontRef font, SDL_Rect rect);
		void SetTitle(const char* title);
		void SetImage(ImageRef image);

		SDL_Rect GetClientRect() const;
		SDL_Rect GetTitleBarRect() const;
		SDL_Rect GetWindowRect() const { return m_rect; };

		HitZone HitTest(SDL_Point);

		void Draw(bool active = false);

		void SetVisible(bool visible = true) { m_visible = visible; }
		bool IsVisible() const { return m_visible;  }

	protected:
		Window(SDL::RendererRef renderer, SDL::FontRef font, SDL_Rect rect);

		void SetDrawColor(const GUI::Color & col);
		void Draw3dFrame(SDL_Rect pos, bool raised);
		void DrawReliefBox(SDL_Rect pos, const GUI::Color & col, bool raised);
		void DrawButton(SDL_Rect pos, const GUI::Color & col, bool raised);
		void DrawTitleBar(bool active);
		void DrawTitle(bool active);
		void RenderTitle();
		SDL::TexturePtr SurfaceToTexture(SDL_Surface* surf);

		std::string m_title;
		SDL_Rect m_rect;

		static uint8_t constexpr m_borderWidth = 6;
		static uint8_t constexpr m_buttonSize = 24;

		SDL::RendererRef m_renderer;
		SDL::FontRef m_font;
		static ImagePtr m_titleBackground;
		SDL::TexturePtr m_activeTitle;
		SDL::TexturePtr m_inactiveTitle;
		SDL_Rect m_titleStrRect;
		ImageRef m_image;

		bool m_visible;

		struct shared_enabler;
	};
}