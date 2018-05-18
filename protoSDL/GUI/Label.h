#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "WindowManager.h"
#include <string>
#include <vector>

namespace GUI
{
	class Label : public Widget
	{
	public:
		enum TextAlign : uint8_t {
			TEXT_H_LEFT = 1,
			TEXT_H_RIGHT = 2,
			TEXT_H_CENTER = TEXT_H_LEFT | TEXT_H_RIGHT,

			TEXT_V_TOP = 8,
			TEXT_V_BOTTOM = 16,
			TEXT_V_CENTER = TEXT_V_TOP | TEXT_V_BOTTOM,

			TEXT_DEFAULT = TEXT_H_CENTER | TEXT_V_CENTER
		};

		virtual ~Label() = default;
		Label(const Button&) = delete;
		Label& operator=(const Label&) = delete;
		Label(Label&&) = delete;
		Label& operator=(Label&&) = delete;

		void Init() override;

		static LabelPtr Create(const char* id, RendererRef renderer, Rect rect, const char* label, FontRef font = nullptr);

		void Draw() override;

		void SetText(const char *) override;

		void SetAlign(uint8_t align);
		uint8_t GetAlign() { return m_labelAlign;  }

	protected:
		Label(const char* id, RendererRef renderer, Rect rect, const char* label, FontRef font);

		void RenderLabel();
		void DrawLabel(RectRef rect);

		TexturePtr m_labelText;
		Rect m_labelRect;
		TextAlign m_labelAlign;

		struct shared_enabler;
	};
}