#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "Point.h"
#include <string>
#include <vector>

namespace GUI
{
	class Toolbar : public Widget
	{
	public:
		DECLARE_EVENT_CLASS_NAME(Toolbar)

		enum ToolbarEvents : EventCode
		{
			EVENT_TOOLBAR_CLICKED // ToolbarItemRef in data2
		};

		using ToolbarItems = std::vector<ToolbarItemPtr>;

		virtual ~Toolbar() = default;
		Toolbar(const Toolbar&) = delete;
		Toolbar& operator=(const Toolbar&) = delete;
		Toolbar(Toolbar&&) = delete;
		Toolbar& operator=(Toolbar&&) = delete;

		void Init() override;

		static ToolbarPtr Create(RendererRef renderer, const char * id, int height);

		ToolbarItemPtr AddToolbarItem(const char * id, ImageRef image, const char * name = nullptr);
		void AddSeparator();

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;

		void Draw() override {};
		void Draw(const RectRef);

		void SetFocus(WidgetRef focus, WidgetRef parent = nullptr) override;

		int GetHeight(int clientWidth) const;

	protected:
		Toolbar(RendererRef renderer, const char * id, int height);

		ToolbarItemPtr ItemAt(PointRef pt);

		ToolbarItems::const_iterator FindToolbarItem(ToolbarItemRef item) const;
		
		ToolbarItems m_items;
		int m_height;

		struct shared_enabler;
	};
}