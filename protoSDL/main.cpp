#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Common.h"
#include "GUI\Window.h"
#include "GUI\WindowManager.h"
#include "GUI\ResourceManager.h"
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

void ToggleFullscreen(SDL::MainWindowPtr & window) {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool IsFullscreen = SDL_GetWindowFlags(window.get()) & FullscreenFlag;
	SDL_SetWindowFullscreen(window.get(), IsFullscreen ? 0 : FullscreenFlag);
	SDL_ShowCursor(1);
}

void DeleteTexture(::SDL_Texture* surface)
{
	SDL_DestroyTexture(surface);
}

SDL::MainWindowPtr CreateWindow(char const *title, int x, int y, int w, int h, Uint32 flags)
{
	return SDL::MainWindowPtr(SDL_CreateWindow(title, x, y, w, h, flags), SDL::sdl_deleter());
}

SDL::RendererPtr CreateRenderer(SDL::MainWindowPtr & window, int index, Uint32 flags)
{
	return SDL::RendererPtr(SDL_CreateRenderer(window.get(), index, flags), SDL::sdl_deleter());
}

GUI::WindowPtr window;

void Render(SDL::RendererPtr & ren)
{	
	SDL_SetRenderDrawColor(ren.get(), 64, 64, 64, 255);
	
	SDL_RenderClear(ren.get());

	GUI::WindowManager::Get().Draw();

	SDL_RenderPresent(ren.get());
}

SDL_Texture* SurfaceToTexture(SDL::RendererPtr & ren, SDL_Surface* surf)
{
	SDL_Texture* text;

	text = SDL_CreateTextureFromSurface(ren.get(), surf);

	SDL_FreeSurface(surf);

	return text;
}

int main(int argc, char ** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if (TTF_Init() == -1)
	{
		std::cout << " Failed to initialize TTF : " << SDL_GetError() << std::endl;
		return 1;
	}

	//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
	//	"Missing file",
	//	"File is missing. Please reinstall the program.",
	//	NULL);

	{
		SDL::MainWindowPtr win = CreateWindow("Hello SDL World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);

		SDL::RendererPtr ren = CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == nullptr)
		{
			SDL_DestroyWindow(win.get());
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return 1;
		}

		GUI::ResourceManager::Get().Init(ren);
		GUI::WindowManager::Get().Init(ren);

		TTF_Init();
		SDL::FontRef font = GUI::ResourceManager::Get().LoadFont("default", "./Resources/Oxygen-Bold.ttf", 14);
		GUI::ResourceManager::Get().LoadFont("mono", "./Resources/FiraMono-Regular.ttf", 14);
		GUI::ImageRef image = GUI::ResourceManager::Get().LoadImage("iconChip", "./Resources/iconChip.png");

		GUI::WindowPtr mainWnd = GUI::WindowManager::Get().AddWindow("main", { 0, 0, 1280, 720 });
		mainWnd->SetTitle("DIGI-SIM");
		mainWnd->SetImage(image);
		SDL_Rect client = mainWnd->GetClientRect();

		GUI::WindowManager::Get().AddWindow("nedit", { client.x, client.y, client.w - 300, client.h - 200 })->SetTitle("Editor");
		GUI::WindowManager::Get().AddWindow("sim", { client.x, client.y + client.h - 200, client.w - 300, 200 })->SetTitle("Simulation");
		GUI::WindowManager::Get().AddWindow("parts", { client.x + client.w - 300, client.y, 300, client.h })->SetTitle("Parts Bin");

		SDL_Cursor* handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		SDL_Cursor* normalCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		SDL_Cursor* sizeAllCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		SDL_SetCursor(normalCursor);

		Render(ren);

		SDL_Event e;
		bool quit = false;
		while (!quit) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
					SDL_Point pt = { e.button.x, e.button.y };
					GUI::WindowPtr hit = GUI::WindowManager::HitTest(pt);
					if (hit)
					{
						switch (hit->HitTest(pt))
						{
						case GUI::Window::HIT_TITLEBAR:
							SDL_SetCursor(sizeAllCursor);
							break;
						default:
							SDL_SetCursor(normalCursor);
						}
						Render(ren);
					}

				}
				else if (e.type == SDL_MOUSEBUTTONDOWN) {
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						SDL_Point pt = { e.button.x, e.button.y };
						GUI::WindowPtr hit = GUI::WindowManager::HitTest(pt);
						if (hit)
						{
							GUI::WindowManager::SetActive(hit);
							Render(ren);
						}
					}
				}
				else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {

					case SDLK_RETURN:
						if (SDL_GetModState() & KMOD_ALT)
						{
							ToggleFullscreen(win);
							Render(ren);
						}
						break;
					case SDLK_q: quit = true; break;
					}
				}
			}
			//Render the scene
		}
	}

	SDL_Quit();

	return 0;
}
