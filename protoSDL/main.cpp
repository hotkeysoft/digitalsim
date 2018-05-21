#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Common.h"
#include "GUI\Window.h"
#include "GUI\WindowManager.h"
#include "GUI\ResourceManager.h"
#include "GUI\Button.h"
#include "GUI\Label.h"
#include "GUI\TextBox.h"
#include "GUI\Tree.h"
#include <string>
#include <iostream>
#include <memory>
#include <cassert>
#include <sstream>
#include <fstream>

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

void OnClick(WidgetRef widget)
{
	int pos = 0;
	int col = 0;
	if (widget->GetId() == "b2")
	{
		pos = 2;
		col = 2;
	}
	else if (widget->GetId() == "b3")
	{
		pos = 9999;
		col = 9999;
	}
	std::static_pointer_cast<TextBox>(WINMGR().FindWindow("sim")->FindControl("text"))->InsertAt("#", pos, col);
}

int main(int argc, char ** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
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
		RES().LoadFont("big", "./Resources/Oxygen-Regular.ttf", 18);
		ImageRef image = RES().LoadImage("iconChip", "./Resources/iconChip.png");
		RES().LoadImage("win.maximize", "./Resources/iconMaximize.png");
		RES().LoadImage("win.minimize", "./Resources/iconMinimize.png");
		RES().LoadImage("win.restore", "./Resources/iconRestore.png");
		RES().LoadImage("win.scroll.left", "./Resources/iconLeftArrow.png");
		RES().LoadImage("win.scroll.right", "./Resources/iconRightArrow.png");
		RES().LoadImage("win.scroll.up", "./Resources/iconUpArrow.png");
		RES().LoadImage("win.scroll.down", "./Resources/iconDownArrow.png");

		RES().LoadImage("iconSim", "./Resources/iconSim.png");

		std::string sampleText;

		{
			std::ifstream t("./Resources/sample.txt");
			std::stringstream buffer;
			buffer << t.rdbuf();
			sampleText = buffer.str();
		}

		RES().LoadCursor("edit.ibeam", SDL_SYSTEM_CURSOR_IBEAM);

		CursorRef sizeNWSECursor = RES().LoadCursor("size.NWSE", SDL_SYSTEM_CURSOR_SIZENWSE);
		CursorRef sizeNESWCursor = RES().LoadCursor("size.NESW", SDL_SYSTEM_CURSOR_SIZENESW);
		CursorRef sizeWECursor = RES().LoadCursor("size.WE", SDL_SYSTEM_CURSOR_SIZEWE);
		CursorRef sizeNSCursor = RES().LoadCursor("size.NS", SDL_SYSTEM_CURSOR_SIZENS);

		CursorRef normalCursor = RES().LoadCursor("default", SDL_SYSTEM_CURSOR_ARROW);
		SDL_SetCursor(normalCursor);


		WindowPtr mainWnd = WINMGR().AddWindow("main", { 0, 0, 1280, 720 }, WindowFlags::WIN_SYSMENU | WindowFlags::WIN_ACTIVE | WindowFlags::WIN_NOSCROLL);
		mainWnd->SetText("DIGI-SIM");
		mainWnd->SetImage(image);
		Rect client = mainWnd->GetClientRect();

		WINMGR().AddWindow("edit", mainWnd, { 0, 0, client.w - 300, client.h - 200 })->SetText("Editor");

		WindowPtr editWnd = WINMGR().FindWindow("edit");
		WINMGR().AddWindow("edit.1", editWnd, { 0, 0, 650, 400 })->SetText("edit.1");
		WINMGR().AddWindow("edit.2", editWnd, { 650, 0, 300, 400 })->SetText("edit.2");

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
		simWnd->AddControl(GUI::TextBox::CreateFill("text", ren.get(), sampleText.c_str()));

		WINMGR().AddWindow("parts", mainWnd, { client.w-300, 0, 300, client.h})->SetText("Parts");

		{
			ImageRef opened = RES().FindImage("win.scroll.down");
			ImageRef closed = RES().FindImage("win.scroll.right");

			TreePtr tree = GUI::Tree::Create("tree", ren.get(), 20);
			WINMGR().FindWindow("parts")->AddControl(tree);

			TreeNodeRef root = tree->AddNode("Root");
			
			TreeNodeRef l1 = tree->AddNode("1", opened, closed, root);
			TreeNodeRef l2 = tree->AddNode("2", opened, closed, root);
			TreeNodeRef l3 = tree->AddNode("3", opened, closed, root);
			tree->OpenNode(l3, false);

			TreeNodeRef l21 = tree->AddNode("2.1", opened, closed, l2);
			TreeNodeRef l22 = tree->AddNode("2.2", opened, closed, l2);
			tree->OpenNode(l22, false);

			TreeNodeRef l221 = tree->AddNode("2.2.1", opened, closed, l22);

			TreeNodeRef l31 = tree->AddNode("3.1", opened, closed, l3);
			TreeNodeRef l32 = tree->AddNode("3.2", opened, closed, l3);
			TreeNodeRef l311 = tree->AddNode("3.1.1", opened, closed, l31);
			TreeNodeRef l312 = tree->AddNode("3.1.2", opened, closed, l31);
			TreeNodeRef l313 = tree->AddNode("3.1.3", opened, closed, l31);
			TreeNodeRef l3123 = tree->AddNode("3.1.2.3", opened, closed, l312);
			
		}	
		
		WindowPtr e2 = WINMGR().FindWindow("edit.2");
		e2->AddControl(GUI::Button::Create("b1", ren.get(), Rect(100, 30, 55, 24), "Button"));
		e2->AddControl(GUI::Button::Create("b2", ren.get(), Rect(50, 60, 110, 24), "Another Button"));
		e2->AddControl(GUI::Button::Create("b3", ren.get(), Rect(225, 30, 90, 70), "A Big Button"));
		e2->AddControl(GUI::Button::Create("b4", ren.get(), Rect(500, 100, 220, 24), "A button far far away"));
		
		{
			LabelPtr label = GUI::Label::CreateSingle("alabel", ren.get(), Rect(100, 8, 55, 24), "A label");
			e2->AddControl(label);
		}

		{
			TextBoxPtr box = GUI::TextBox::CreateSingleLine("txt", ren.get(), Rect(50, 90, 155, 24), "Text");
			e2->AddControl(box);
		}
		{
			TextBoxPtr box = GUI::TextBox::CreateSingleLine("txt2", ren.get(), Rect(50, 116, 155, 40), "123456789012345");
			box->SetPadding(10);
			box->SetMargin(0);
			box->SetBorderWidth(2);
			box->SetBorder(true);
			e2->AddControl(box);
		}


		std::static_pointer_cast<Button>(e2->FindControl("b1"))->SetOnClickHandler(OnClick);
		std::static_pointer_cast<Button>(e2->FindControl("b2"))->SetOnClickHandler(OnClick);
		std::static_pointer_cast<Button>(e2->FindControl("b3"))->SetOnClickHandler(OnClick);
		std::static_pointer_cast<Button>(e2->FindControl("b4"))->SetOnClickHandler(OnClick);

		Rect rect = WINMGR().FindWindow("edit.1.1")->GetClientRect(true, false);

		LabelPtr l1 = GUI::Label::CreateFill("l1", ren.get(), "HLeft, VTop, b0m5");
		l1->SetBorder(false);
		l1->SetPadding(5);
		l1->SetAlign(Label::TEXT_H_LEFT | Label::TEXT_V_TOP);

		LabelPtr l2 = GUI::Label::CreateFill("l2", ren.get(), "HCenter, VTop, b1");
		l2->SetBorder(true);
		l2->SetAlign(Label::TEXT_H_CENTER | Label::TEXT_V_TOP);

		LabelPtr l3 = GUI::Label::CreateFill("l3", ren.get(), "HRight, VTop, b2p5");
		l3->SetBorder(true);
		l3->SetPadding(5);
		l3->SetBorderWidth(2);
		l3->SetAlign(Label::TEXT_H_RIGHT | Label::TEXT_V_TOP);

		LabelPtr l4 = GUI::Label::CreateFill("l4", ren.get(), "HLeft VCenter, b0");
		l4->SetBorder(false);
		l4->SetBorderWidth(1);
		l4->SetAlign(Label::TEXT_H_LEFT | Label::TEXT_V_CENTER);

		LabelPtr l5 = GUI::Label::CreateFill("l5", ren.get(), "HCenter, VCenter, b0");
		l5->SetBorder(false);
		l5->SetAlign(Label::TEXT_H_CENTER | Label::TEXT_V_CENTER);

		LabelPtr l6 = GUI::Label::CreateFill("l6", ren.get(), "HRight, VCenter, b1p5m5");
		l6->SetBorder(true);
		l6->SetMargin(5);
		l6->SetPadding(5);
		l6->SetBorderWidth(1);
		l6->SetAlign(Label::TEXT_H_RIGHT | Label::TEXT_V_CENTER);

		LabelPtr l7 = GUI::Label::CreateFill("l7", ren.get(), "HLeft, VBottom, b2");
		l7->SetBorder(true);
		l7->SetBorderWidth(2);
		l7->SetAlign(Label::TEXT_H_LEFT | Label::TEXT_V_BOTTOM);

		LabelPtr l8 = GUI::Label::CreateFill("l8", ren.get(), "HCenter, VBottom, b4m4");
		l8->SetBorder(true);
		l8->SetBorderWidth(4);
		l8->SetMargin(4);		
		l8->SetAlign(Label::TEXT_H_CENTER | Label::TEXT_V_BOTTOM);

		LabelPtr l9 = GUI::Label::CreateFill("l9", ren.get(), "HRight, VBottom, b2p5");
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

		SDL_StartTextInput();
		SDL_Event e;
		bool quit = false;
		while (!quit) 
		{
			while (SDL_PollEvent(&e)) 
			{
				if (e.type == SDL_QUIT) 
				{
					quit = true;
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
					if (WINMGR().GetCapture())
					{
						WINMGR().GetCapture().Target.target->HandleEvent(&e);
						Render(ren);
					}
					else
					{
						Point pt(e.button.x, e.button.y);
						HitResult hit = WINMGR().HitTest(&pt);
						if (hit && hit.target->HandleEvent(&e))
						{
							Render(ren);
						}
					}
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN) {
					if (e.button.button == SDL_BUTTON_LEFT)
					{
						Point pt(e.button.x, e.button.y);
						HitResult hit = WINMGR().HitTest(&pt);
						if (hit)
						{	
							hit.target->SetActive();
							hit.target->HandleEvent(&e);
							Render(ren);
						}
					}
				}
				//else if (e.type == SDL_MOUSEBUTTONUP/* || e.type == SDL_KEYUP*/) 
				//{
				//}
				else if (e.type == SDL_KEYDOWN) 
				{
					// Pass to active window first
					if (!WINMGR().GetActive()->HandleEvent(&e))
					{
						switch (e.key.keysym.sym) 
						{
						case SDLK_RETURN:
							if (SDL_GetModState() & KMOD_ALT)
							{
								ToggleFullscreen(win);
								Render(ren);
							}
							break;
						}
					}
					else
					{
						Render(ren);
					}
				}
				else if (WINMGR().GetCapture() && WINMGR().GetCapture().Target.target->HandleEvent(&e))
				{
					Render(ren);
				}
				else // Pass to active window
				{
					WINMGR().GetActive()->HandleEvent(&e);
					Render(ren);
				}	
			}
			SDL_WaitEvent(nullptr);
//			SDL_Delay(2);
		}
	}

	SDL_Quit();

	return 0;
}
