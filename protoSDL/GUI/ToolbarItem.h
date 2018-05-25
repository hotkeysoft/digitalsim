#pragma once
#include "Common.h"
#include "Widget.h"
#include "Button.h"
#include "Rect.h"
#include "Color.h"
#include "WindowManager.h"
#include <string>
#include <vector>

namespace GUI
{
	class ToolbarItem : public Button
	{
	public:
		DECLARE_EVENT_CLASS_NAME(ToolbarItem)

		virtual ~ToolbarItem() = default;
		ToolbarItem(const ToolbarItem&) = delete;
		ToolbarItem& operator=(const ToolbarItem&) = delete;
		ToolbarItem(ToolbarItem&&) = delete;
		ToolbarItem& operator=(ToolbarItem&&) = delete;

		void Init() override;

		static ToolbarItemPtr Create(const char* id, RendererRef renderer, const char* label, ImageRef image = nullptr, FontRef font = nullptr);

		HitResult HitTest(const PointRef pt) override;

		void Draw(const RectRef);

	protected:
		ToolbarItem(const char* id, RendererRef renderer, const char* label, ImageRef image, FontRef font);

		Rect m_drawLocation;

		struct shared_enabler;
	};
}