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

	WINMGR().Draw();

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
		RES().LoadImage("win.maximize", "./Resources/iconMaximize.png");
		RES().LoadImage("win.minimize", "./Resources/iconMinimize.png");
		RES().LoadImage("win.restore", "./Resources/iconRestore.png");

		WindowPtr mainWnd = WINMGR().AddWindow("main", { 0, 0, 1280, 720 }, WindowFlags::WIN_SYSMENU);
		mainWnd->SetTitle("DIGI-SIM");
		mainWnd->SetImage(image);
		SDL_Rect client = mainWnd->GetClientRect();

		WINMGR().AddWindow("edit", mainWnd, { 0, 0, client.w - 300, client.h - 200 })->SetTitle("Editor");

		WindowPtr editWnd = WINMGR().FindWindow("edit");
		WINMGR().AddWindow("edit.1", editWnd, { 0, 0, 400, 200 })->SetTitle("edit.1");
		WINMGR().AddWindow("edit.2", editWnd, { 400, 0, 100, 100 }, WindowFlags::WIN_CANRESIZE| WindowFlags::WIN_CANMOVE)->SetTitle("edit.2");

		WindowPtr edit1Wnd = WINMGR().FindWindow("edit.1");
		WINMGR().AddWindow("edit.1.1", edit1Wnd, { 0, 0, 100, 100 })->SetTitle("edit.1.1  With a long name");

		WINMGR().AddWindow("sim", mainWnd, { 0, client.h - 200, client.w - 300, 200 })->SetTitle("Simulation");
		WindowPtr simWnd = WINMGR().FindWindow("sim");

		WINMGR().AddWindow("parts", mainWnd, { client.w - 300, 0, 300, client.h })->SetTitle("Parts Bin");

		CursorRef sizeNWSECursor = RES().LoadCursor("size.NWSE", SDL_SYSTEM_CURSOR_SIZENWSE);
		CursorRef sizeNESWCursor = RES().LoadCursor("size.NESW", SDL_SYSTEM_CURSOR_SIZENESW);
		CursorRef sizeWECursor = RES().LoadCursor("size.WE", SDL_SYSTEM_CURSOR_SIZEWE);
		CursorRef sizeNSCursor = RES().LoadCursor("size.NS", SDL_SYSTEM_CURSOR_SIZENS);

		CursorRef normalCursor = RES().LoadCursor("default", SDL_SYSTEM_CURSOR_ARROW);
		SDL_SetCursor(normalCursor);



		Render(ren);

		bool mouseCaptured = false;
		WindowPtr captureTarget = nullptr;
		HitZone captureZone = HIT_NOTHING;
		SDL_Point lastPos;

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
					if (mouseCaptured && captureTarget)
					{
						SDL_Point rel = { pt.x - lastPos.x, pt.y - lastPos.y };
						switch (captureZone)
						{
						case HIT_TITLEBAR:
							captureTarget->MoveRel(rel);
							break;
						case HIT_BORDER_LEFT:
							if (captureTarget->MoveRel(SDL_Point({ rel.x, 0 })))
							{
								captureTarget->ResizeRel(SDL_Point({ -rel.x, 0 }));
							}
							break;
						case HIT_BORDER_RIGHT:
							captureTarget->ResizeRel({ rel.x, 0 });
							break;
						case HIT_BORDER_TOP:
							if (captureTarget->MoveRel(SDL_Point({ 0, rel.y })))
							{
								captureTarget->ResizeRel({ 0, -rel.y });
							}
							break;
						case HIT_BORDER_BOTTOM:
							captureTarget->ResizeRel({ 0, rel.y });
							break;
						case HIT_CORNER_TOPLEFT:
							if (captureTarget->MoveRel(rel))
							{
								captureTarget->ResizeRel({ -rel.x, -rel.y });
							}
							break;
						case HIT_CORNER_TOPRIGHT:
							if (captureTarget->MoveRel(SDL_Point({ 0, rel.y })))
							{
								captureTarget->ResizeRel({ rel.x, -rel.y });
							}
							break;
						case HIT_CORNER_BOTTOMLEFT:
							if (captureTarget->MoveRel(SDL_Point({ rel.x, 0 })))
							{
								captureTarget->ResizeRel({ -rel.x, rel.y });
							}
							break;
						case HIT_CORNER_BOTTOMRIGHT:
							if (captureTarget->MoveRel(SDL_Point({ 0, 0 })))
							{
								captureTarget->ResizeRel({ rel.x, rel.y });
							}
							break;
						case HIT_SYSMENU:
						case HIT_MAXBUTTON:
						case HIT_MINBUTTON:
							captureTarget->ToggleButtonState(captureZone, captureTarget->HitTest(pt) == captureZone);
							break;
						}
						lastPos = pt;
						Render(ren);
					}
					else
					{
						WindowPtr hit = WINMGR().HitTest(pt);
						if (hit)
						{
							switch (hit->HitTest(pt))
							{
							case HIT_BORDER_TOP:
							case HIT_BORDER_BOTTOM:
								SDL_SetCursor(sizeNSCursor);
								break;
							case HIT_BORDER_LEFT:
							case HIT_BORDER_RIGHT:
								SDL_SetCursor(sizeWECursor);
								break;
							case HIT_CORNER_TOPLEFT:
							case HIT_CORNER_BOTTOMRIGHT:
								SDL_SetCursor(sizeNWSECursor);
								break;
							case HIT_CORNER_TOPRIGHT:
							case HIT_CORNER_BOTTOMLEFT:
								SDL_SetCursor(sizeNESWCursor);
								break;
							default:
								SDL_SetCursor(normalCursor);
							}
							Render(ren);
						}
					}

				}
				else if (e.type == SDL_MOUSEBUTTONDOWN) {
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						SDL_Point pt = { e.button.x, e.button.y };
						WindowPtr hit = WINMGR().HitTest(pt);
						if (hit)
						{
							WINMGR().SetActive(hit);
							captureZone = hit->HitTest(pt);
							if (captureZone == HIT_SYSMENU || 
								captureZone == HIT_MINBUTTON ||
								captureZone == HIT_MAXBUTTON)
							{
								hit->ToggleButtonState(captureZone, true);
								mouseCaptured = true;
								captureTarget = hit;
								lastPos = pt;
							}
							else if (captureZone == HIT_TITLEBAR)
							{
								mouseCaptured = true;
								captureTarget = hit;
								lastPos = pt;
							}
							else if (captureZone & HIT_BORDER_ANY | HIT_CORNER_ANY)
							{
								mouseCaptured = true;
								captureTarget = hit;
								lastPos = pt;
							}
						}
						Render(ren);
					}
				}
				else if (e.type == SDL_MOUSEBUTTONUP) {
					if (mouseCaptured)
					{
						SDL_Point pt = { e.button.x, e.button.y };
						switch (captureZone)
						{
						case HIT_SYSMENU:
						case HIT_MAXBUTTON:
						case HIT_MINBUTTON:
							captureTarget->ToggleButtonState(captureZone, false);
							if (captureTarget->HitTest(pt) == captureZone)
							{
								captureTarget->ButtonPushed(captureZone);
							}
							break;
						}

						mouseCaptured = false;
						captureTarget = nullptr;
						Render(ren);
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
			SDL_Delay(2);
		}
	}

	SDL_Quit();

	return 0;
}
