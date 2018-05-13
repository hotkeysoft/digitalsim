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

using namespace GUI;

void ToggleFullscreen(MainWindowPtr & window) {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool IsFullscreen = SDL_GetWindowFlags(window.get()) & FullscreenFlag;
	SDL_SetWindowFullscreen(window.get(), IsFullscreen ? 0 : FullscreenFlag);
	SDL_ShowCursor(1);
}

MainWindowPtr CreateWindow(char const *title, int x, int y, int w, int h, Uint32 flags)
{
	return MainWindowPtr(SDL_CreateWindow(title, x, y, w, h, flags), sdl_deleter());
}

RendererPtr CreateRenderer(MainWindowPtr & window, int index, Uint32 flags)
{
	return RendererPtr(SDL_CreateRenderer(window.get(), index, flags), sdl_deleter());
}

void Render(RendererPtr & ren)
{	
	SDL_SetRenderDrawColor(ren.get(), 64, 64, 64, 255);
	
	SDL_RenderClear(ren.get());

	GUI::WINMGR().Draw();

	SDL_RenderPresent(ren.get());
}

SDL_Texture* SurfaceToTexture(RendererPtr & ren, SDL_Surface* surf)
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
		MainWindowPtr win = CreateWindow("Hello SDL World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);

		RendererPtr ren = CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == nullptr)
		{
			SDL_DestroyWindow(win.get());
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return 1;
		}

		RES().Init(ren);
		WINMGR().Init(ren);

		TTF_Init();
		FontRef font = RES().LoadFont("default", "./Resources/Oxygen-Bold.ttf", 14);
		RES().LoadFont("mono", "./Resources/FiraMono-Regular.ttf", 14);
		ImageRef image = RES().LoadImage("iconChip", "./Resources/iconChip.png");

		WindowPtr mainWnd = WINMGR().AddWindow("main", { 0, 0, 1280, 720 });
		mainWnd->SetTitle("DIGI-SIM");
		mainWnd->SetImage(image);
		SDL_Rect client = mainWnd->GetClientRect();

		WINMGR().AddWindow("nedit", mainWnd, { 0, 0, client.w - 300, client.h - 200 })->SetTitle("Editor");
//		WINMGR().AddWindow("nedit", mainWnd, { client.x, client.y, client.w - 300, client.h - 200 })->SetTitle("Editor");
//		WINMGR().AddWindow("sim", mainWnd, { client.x, client.y + client.h - 200, client.w - 300, 200 })->SetTitle("Simulation");
//		WINMGR().AddWindow("parts", mainWnd, { client.x + client.w - 300, client.y, 300, client.h })->SetTitle("Parts Bin");

		CursorRef handCursor = RES().LoadCursor("hand", SDL_SYSTEM_CURSOR_HAND);
		CursorRef normalCursor = RES().LoadCursor("default", SDL_SYSTEM_CURSOR_ARROW);
		CursorRef sizeAllCursor = RES().LoadCursor("size.all", SDL_SYSTEM_CURSOR_SIZEALL);
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
					WindowPtr hit = WINMGR().HitTest(pt);
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
						GUI::WindowPtr hit = WINMGR().HitTest(pt);
						if (hit)
						{
							WINMGR().SetActive(hit);
							Render(ren);
						}
					}
				}
				else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
					case SDLK_LEFT:
						WINMGR().GetActive()->MoveRel(SDL_Point({ -2, 0 })); Render(ren); break;
					case SDLK_RIGHT:
						WINMGR().GetActive()->MoveRel(SDL_Point({ 2, 0 })); Render(ren); break;
					case SDLK_UP:
						WINMGR().GetActive()->MoveRel(SDL_Point({ 0, -2 })); Render(ren); break;
					case SDLK_DOWN:
						WINMGR().GetActive()->MoveRel(SDL_Point({ 0, 2 })); Render(ren); break;

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
		}
	}

	SDL_Quit();

	return 0;
}
