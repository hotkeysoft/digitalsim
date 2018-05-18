#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Common.h"
#include "GUI\Window.h"
#include "GUI\WindowManager.h"
#include "GUI\ResourceManager.h"
#include "GUI\Button.h"
#include "GUI\Label.h"
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

void OnClick(WidgetRef widget)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
		widget->GetId().c_str(),
		"Click!",
		NULL);
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

		RES().LoadFont("default", "./Resources/Oxygen-Regular.ttf", 14);
		RES().LoadFont("win.title", "./Resources/Oxygen-Bold.ttf", 14);
		RES().LoadFont("mono", "./Resources/FiraMono-Regular.ttf", 14);
		ImageRef image = RES().LoadImage("iconChip", "./Resources/iconChip.png");
		RES().LoadImage("win.maximize", "./Resources/iconMaximize.png");
		RES().LoadImage("win.minimize", "./Resources/iconMinimize.png");
		RES().LoadImage("win.restore", "./Resources/iconRestore.png");
		RES().LoadImage("win.scroll.left", "./Resources/iconLeftArrow.png");
		RES().LoadImage("win.scroll.right", "./Resources/iconRightArrow.png");
		RES().LoadImage("win.scroll.up", "./Resources/iconUpArrow.png");
		RES().LoadImage("win.scroll.down", "./Resources/iconDownArrow.png");

		RES().LoadImage("iconSim", "./Resources/iconSim.png");

		WindowPtr mainWnd = WINMGR().AddWindow("main", { 0, 0, 1280, 720 }, WindowFlags::WIN_SYSMENU | WindowFlags::WIN_ACTIVE | WindowFlags::WIN_NOSCROLL);
		mainWnd->SetText("DIGI-SIM");
		mainWnd->SetImage(image);
		Rect client = mainWnd->GetClientRect();

		WINMGR().AddWindow("edit", mainWnd, { 0, 0, client.w - 300, client.h - 200 })->SetText("Editor");

		WindowPtr editWnd = WINMGR().FindWindow("edit");
		WINMGR().AddWindow("edit.1", editWnd, { 0, 0, 650, 400 })->SetText("edit.1");
		WINMGR().AddWindow("edit.2", editWnd, { 650, 0, 200, 400 })->SetText("edit.2");

		WindowPtr edit1Wnd = WINMGR().FindWindow("edit.1");
		WINMGR().AddWindow("edit.1.1", edit1Wnd, { 0, 0, 200, 100 }, WindowFlags::WIN_CANRESIZE | WindowFlags::WIN_CANMOVE)->SetText("edit.1.1  With a long name");
		WINMGR().AddWindow("edit.1.2", edit1Wnd, { 200, 0, 200, 100 })->SetText("edit.1.2  With a long name");
		WINMGR().AddWindow("edit.1.3", edit1Wnd, { 400, 0, 200, 100 })->SetText("edit.1.3  With a long name");
		WINMGR().AddWindow("edit.1.4", edit1Wnd, { 0, 100, 200, 100 })->SetText("edit.1.4");
		WINMGR().AddWindow("edit.1.5", edit1Wnd, { 200, 100, 200, 100 })->SetText("edit.1.5");
		WINMGR().AddWindow("edit.1.6", edit1Wnd, { 400, 100, 200, 100 })->SetText("edit.1.6");
		WINMGR().AddWindow("edit.1.7", edit1Wnd, { 0, 200, 200, 100 })->SetText("edit.1.7");
		WINMGR().AddWindow("edit.1.8", edit1Wnd, { 200, 200, 200, 100 })->SetText("edit.1.8");
		WINMGR().AddWindow("edit.1.9", edit1Wnd, { 400, 200, 200, 100 })->SetText("edit.1.9");


		WINMGR().AddWindow("sim", mainWnd, { 0, client.h - 200, client.w - 300, 200 })->SetText("Simulation");
		WindowPtr simWnd = WINMGR().FindWindow("sim");
		simWnd->SetImage(RES().FindImage("iconSim"));

		WINMGR().AddWindow("parts", mainWnd, { client.w - 300, 0, 300, client.h })->SetText("Parts Bin");

		CursorRef sizeNWSECursor = RES().LoadCursor("size.NWSE", SDL_SYSTEM_CURSOR_SIZENWSE);
		CursorRef sizeNESWCursor = RES().LoadCursor("size.NESW", SDL_SYSTEM_CURSOR_SIZENESW);
		CursorRef sizeWECursor = RES().LoadCursor("size.WE", SDL_SYSTEM_CURSOR_SIZEWE);
		CursorRef sizeNSCursor = RES().LoadCursor("size.NS", SDL_SYSTEM_CURSOR_SIZENS);

		CursorRef normalCursor = RES().LoadCursor("default", SDL_SYSTEM_CURSOR_ARROW);
		SDL_SetCursor(normalCursor);

		WindowPtr e2 = WINMGR().FindWindow("edit.2");
		e2->AddControl(GUI::Button::Create("b1", ren.get(), Rect(100, 30, 55, 24), "Button"));
		e2->AddControl(GUI::Button::Create("b2", ren.get(), Rect(50, 60, 110, 24), "Another Button"));

		e2->AddControl(GUI::Button::Create("b3", ren.get(), Rect(200, 50, 70, 70), "A Big Button"));

		e2->AddControl(GUI::Button::Create("b4", ren.get(), Rect(500, 100, 220, 24), "A button far far away"));

		e2->FindControl("b1")->SetBorderWidth(1);

		std::static_pointer_cast<Button>(e2->FindControl("b1"))->SetOnClickHandler(OnClick);
		std::static_pointer_cast<Button>(e2->FindControl("b2"))->SetOnClickHandler(OnClick);
		std::static_pointer_cast<Button>(e2->FindControl("b3"))->SetOnClickHandler(OnClick);
		std::static_pointer_cast<Button>(e2->FindControl("b4"))->SetOnClickHandler(OnClick);


		Rect rect = WINMGR().FindWindow("edit.1.1")->GetClientRect(true, false);

		LabelPtr l1 = GUI::Label::Create("l1", ren.get(), rect, "HLeft, VTop, b0m5");
		l1->SetBorder(false);
		l1->SetPadding(5);
		l1->SetAlign(Label::TEXT_H_LEFT | Label::TEXT_V_TOP);

		LabelPtr l2 = GUI::Label::Create("l2", ren.get(), rect, "HCenter, VTop, b1");
		l2->SetBorder(true);
		l2->SetAlign(Label::TEXT_H_CENTER | Label::TEXT_V_TOP);

		LabelPtr l3 = GUI::Label::Create("l3", ren.get(), rect, "HRight, VTop, b2p5");
		l3->SetBorder(true);
		l3->SetPadding(5);
		l3->SetBorderWidth(2);
		l3->SetAlign(Label::TEXT_H_RIGHT | Label::TEXT_V_TOP);

		LabelPtr l4 = GUI::Label::Create("l4", ren.get(), rect, "HLeft VCenter, b0");
		l4->SetBorder(false);
		l4->SetBorderWidth(1);
		l4->SetAlign(Label::TEXT_H_LEFT | Label::TEXT_V_CENTER);

		LabelPtr l5 = GUI::Label::Create("l5", ren.get(), rect, "HCenter, VCenter, b0");
		l5->SetBorder(false);
		l5->SetAlign(Label::TEXT_H_CENTER | Label::TEXT_V_CENTER);

		LabelPtr l6 = GUI::Label::Create("l6", ren.get(), rect, "HRight, VCenter, b1p5m5");
		l6->SetBorder(true);
		l6->SetMargin(5);
		l6->SetPadding(5);
		l6->SetBorderWidth(1);
		l6->SetAlign(Label::TEXT_H_RIGHT | Label::TEXT_V_CENTER);

		LabelPtr l7 = GUI::Label::Create("l7", ren.get(), rect, "HLeft, VBottom, b2");
		l7->SetBorder(true);
		l7->SetBorderWidth(2);
		l7->SetAlign(Label::TEXT_H_LEFT | Label::TEXT_V_BOTTOM);

		LabelPtr l8 = GUI::Label::Create("l8", ren.get(), rect, "HCenter, VBottom, b4m4");
		l8->SetBorder(true);
		l8->SetBorderWidth(4);
		l8->SetMargin(4);		
		l8->SetAlign(Label::TEXT_H_CENTER | Label::TEXT_V_BOTTOM);

		LabelPtr l9 = GUI::Label::Create("l9", ren.get(), rect, "HRight, VBottom, b2p5");
		l9->SetBorder(true);
		l9->SetBorderWidth(2);
		l9->SetPadding(5);
		l9->SetAlign(Label::TEXT_H_RIGHT | Label::TEXT_V_BOTTOM);

		WINMGR().FindWindow("edit.1.1")->AddControl(l1);
		WINMGR().FindWindow("edit.1.2")->AddControl(l2);
		WINMGR().FindWindow("edit.1.3")->AddControl(l3);
		WINMGR().FindWindow("edit.1.4")->AddControl(l4);
		WINMGR().FindWindow("edit.1.5")->AddControl(l5);
		WINMGR().FindWindow("edit.1.6")->AddControl(l6);
		WINMGR().FindWindow("edit.1.7")->AddControl(l7);
		WINMGR().FindWindow("edit.1.8")->AddControl(l8);
		WINMGR().FindWindow("edit.1.9")->AddControl(l9);

		WINMGR().FindWindow("edit.1.1")->SetBackgroundColor(Color::C_DARK_GREY);
		WINMGR().FindWindow("edit.1.2")->SetBackgroundColor(Color::C_MED_GREY);
		WINMGR().FindWindow("edit.1.3")->SetBackgroundColor(Color::C_LIGHT_GREY);
		WINMGR().FindWindow("edit.1.4")->SetBackgroundColor(Color::C_WHITE);
		WINMGR().FindWindow("edit.1.5")->SetBackgroundColor(Color::C_DARK_GREY);
		WINMGR().FindWindow("edit.1.6")->SetBackgroundColor(Color::C_MED_GREY);
		WINMGR().FindWindow("edit.1.7")->SetBackgroundColor(Color::C_LIGHT_GREY);
		WINMGR().FindWindow("edit.1.8")->SetBackgroundColor(Color::C_WHITE);
		WINMGR().FindWindow("edit.1.9")->SetBackgroundColor(Color::C_DARK_GREY);

		Render(ren);

		bool mouseCaptured = false;
		HitResult captureGroup;
		Point captureDelta;
		Rect captureOrigin;

		SDL_Event e;
		bool quit = false;
		while (!quit) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					quit = true;
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
					Point pt = { e.button.x, e.button.y };
					WindowRef wnd = dynamic_cast<WindowRef>(captureGroup.target);
					if (mouseCaptured && wnd)
					{
						Point newPos = pt;
						newPos.x += captureDelta.x;
						newPos.y += captureDelta.y;
						Point delta = { (captureOrigin.x - newPos.x) , (captureOrigin.y - newPos.y) };

						switch ((HitZone)captureGroup)
						{
						case HIT_TITLEBAR:
							wnd->MovePos(&newPos);
							break;
						case HIT_BORDER_LEFT:
							wnd->MoveRect(&Rect(newPos.x, captureOrigin.y, captureOrigin.w + delta.x, captureOrigin.h));
							break;
						case HIT_BORDER_RIGHT:
							wnd->Resize(&Point(captureOrigin.w - delta.x, captureOrigin.h));
							break;
						case HIT_BORDER_TOP:
							wnd->MoveRect(&Rect(captureOrigin.x, newPos.y, captureOrigin.w, captureOrigin.h + delta.y));
							break;
						case HIT_BORDER_BOTTOM:
							wnd->Resize(&Point(captureOrigin.w, captureOrigin.h - delta.y));
							break;
						case HIT_CORNER_TOPLEFT:
							wnd->MoveRect(&Rect(newPos.x, newPos.y, captureOrigin.w + delta.x, captureOrigin.h + delta.y));
							break;
						case HIT_CORNER_TOPRIGHT:
							wnd->MoveRect(&Rect(captureOrigin.x, newPos.y, captureOrigin.w - delta.x, captureOrigin.h + delta.y));
							break;
						case HIT_CORNER_BOTTOMLEFT:
							wnd->MoveRect(&Rect(newPos.x, captureOrigin.y, captureOrigin.w + delta.x, captureOrigin.h - delta.y));
							break;
						case HIT_CORNER_BOTTOMRIGHT:
							wnd->Resize(&Point(captureOrigin.w - delta.x, captureOrigin.h - delta.y));
							break;
						case HIT_SYSMENU:
						case HIT_MAXBUTTON:
						case HIT_MINBUTTON:
							wnd->ToggleButtonState(captureGroup, captureGroup.target->HitTest(&pt) == captureGroup);
							break;
						case HIT_HSCROLL_SLIDER:
							wnd->GetScrollBars()->ClickHScrollBar(&pt);
							break;
						case HIT_VSCROLL_SLIDER:
							wnd->GetScrollBars()->ClickVScrollBar(&pt);
							break;
						}
						Render(ren);
					}
					else
					{
						HitResult hit = WINMGR().HitTest(&pt);
						if (hit)
						{
							switch ((HitZone)hit)
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
						Point pt(e.button.x, e.button.y);
						captureGroup = WINMGR().HitTest(&pt);
						if (captureGroup)
						{
							WindowRef wnd = dynamic_cast<WindowRef>(captureGroup.target);
							if (wnd)
							{
								captureOrigin = wnd->GetRect(true, false);
								captureDelta = captureOrigin.Origin();
								captureDelta.x -= pt.x;
								captureDelta.y -= pt.y;

								WINMGR().SetActive(wnd);
								if (captureGroup == HIT_SYSMENU ||
									captureGroup == HIT_MINBUTTON ||
									captureGroup == HIT_MAXBUTTON ||
									captureGroup == HIT_HSCROLL_LEFT ||
									captureGroup == HIT_HSCROLL_RIGHT ||
									captureGroup == HIT_VSCROLL_UP ||
									captureGroup == HIT_VSCROLL_DOWN ||
									captureGroup == HIT_HSCROLL_SLIDER ||
									captureGroup == HIT_VSCROLL_SLIDER)
								{
									wnd->ToggleButtonState(captureGroup, true);
									mouseCaptured = true;
								}
								else if (captureGroup == HIT_HSCROLL_AREA)
								{
									wnd->GetScrollBars()->ClickHScrollBar(&pt);
								}
								else if (captureGroup == HIT_VSCROLL_AREA)
								{
									wnd->GetScrollBars()->ClickVScrollBar(&pt);
								}
								else if (captureGroup == HIT_TITLEBAR)
								{
									mouseCaptured = true;
								}
								else if (captureGroup.zone & (HIT_BORDER_ANY | HIT_CORNER_ANY))
								{
									mouseCaptured = true;
								}
							}
							else if (captureGroup == HIT_CONTROL && captureGroup.target->HandleEvent(&e))
							{
								mouseCaptured = true;
							}
						}
						Render(ren);
					}
				}
				else if (e.type == SDL_MOUSEBUTTONUP) {
					if (mouseCaptured)
					{
						WindowRef wnd = dynamic_cast<WindowRef>(captureGroup.target);
						if (wnd)
						{
							Point pt = { e.button.x, e.button.y };
							switch ((HitZone)captureGroup)
							{
							case HIT_SYSMENU:
							case HIT_MAXBUTTON:
							case HIT_MINBUTTON:
							case HIT_HSCROLL_LEFT:
							case HIT_HSCROLL_RIGHT:
							case HIT_VSCROLL_UP:
							case HIT_VSCROLL_DOWN:
							case HIT_HSCROLL_SLIDER:
							case HIT_VSCROLL_SLIDER:
							{
								wnd->ToggleButtonState(captureGroup, false);
								HitResult finalHit = captureGroup.target->HitTest(&pt);

								if (finalHit == captureGroup)
								{
									wnd->ButtonPushed(captureGroup);
								}
								break;
							}
							}
						}
						else if (captureGroup == HIT_CONTROL)
						{
							captureGroup.target->HandleEvent(&e);
						}

						mouseCaptured = false;
						captureGroup = HIT_NOTHING;
						Render(ren);
					}
				}
				else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
					case SDLK_LEFT:
						WINMGR().GetActive()->GetScrollBars()->ScrollRel(&Point(-2, 0)); Render(ren); break;
					case SDLK_RIGHT:
						WINMGR().GetActive()->GetScrollBars()->ScrollRel(&Point(2, 0)); Render(ren); break;
					case SDLK_UP:
						WINMGR().GetActive()->GetScrollBars()->ScrollRel(&Point(0, -2)); Render(ren); break;
					case SDLK_DOWN:
						WINMGR().GetActive()->GetScrollBars()->ScrollRel(&Point(0, 2)); Render(ren); break;

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
