#pragma once
#include "Common.h"
#include "Core/Widget.h"
#include "Core/Rect.h"
#include "Core/Color.h"
#include "Core/WindowManager.h"
#include <string>
#include <vector>

namespace CoreUI
{
	class Component;
	using ComponentPtr = std::shared_ptr<Component>;
	using ComponentRef = Component * ;

	class Component : public Widget
	{
	public:
		DECLARE_EVENT_CLASS_NAME(Component)

		enum ComponentEvents : EventCode
		{
//			EVENT_BUTTON_CLICKED
		};

		virtual ~Component() = default;
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(Component&&) = delete;

		void Init() override;

		// Regular component at position & size rect
		static ComponentPtr Create(const char* id, RendererRef renderer, Rect rect, const char* label, ImageRef image = nullptr, FontRef font = nullptr, CreationFlags flags = 0);

		//TODO: Ugly
		WindowRef GetParentWnd() const { return static_cast<WindowRef>(m_parent); }

		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;
		void Draw() override;

		void SetText(const char *) override;

	protected:
		Component(const char* id, RendererRef renderer, Rect rect, const char* label, ImageRef image, FontRef font, CreationFlags flags);

		void CreateLabel();
		LabelPtr m_label;

		struct shared_enabler;
	};
}