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

		static ButtonPtr Create(const char* id, RendererRef renderer, Rect rect, const char* label, ImageRef image = nullptr, FontRef font = nullptr);

		HitZone HitTest(const PointRef) override;
		void Draw() override;

	protected:
		Button(const char* id, RendererRef renderer, Rect rect, const char* label, ImageRef image, FontRef font);

		struct shared_enabler;
	};
}