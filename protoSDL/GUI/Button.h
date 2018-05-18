#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Color.h"
#include "WindowManager.h"
#include <string>
#include <vector>

namespace GUI
{
	class Button : public Widget
	{
	public:
		virtual ~Button() = default;
		Button(const Button&) = delete;
		Button& operator=(const Button&) = delete;
		Button(Button&&) = delete;
		Button& operator=(Button&&) = delete;

		void Init() override;

		static ButtonPtr Create(const char* id, RendererRef renderer, Rect rect, const char* label, ImageRef image = nullptr, FontRef font = nullptr);

		Rect GetRect(bool relative = true, bool scrolled = true) const override;
		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;
		void Draw() override;

		void SetText(const char *) override;

		void SetOnClickHandler(Widget::EventHandler f) { m_onClickHandler = f; }

	protected:
		Button(const char* id, RendererRef renderer, Rect rect, const char* label, ImageRef image, FontRef font);

		void CreateLabel();
		LabelPtr m_label;
		bool m_pushed;
		EventHandler m_onClickHandler;

		struct shared_enabler;
	};
}