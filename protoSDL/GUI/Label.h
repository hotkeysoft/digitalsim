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

			TEXT_FILL_DEFAULT = TEXT_H_CENTER | TEXT_V_CENTER,
			TEXT_SINGLE_DEFAULT = TEXT_H_LEFT | TEXT_V_TOP,
			TEXT_AUTOSIZE_DEFAULT = TEXT_H_LEFT | TEXT_V_TOP,
		};

		virtual ~Label() = default;
		Label(const Label&) = delete;
		Label& operator=(const Label&) = delete;
		Label(Label&&) = delete;
		Label& operator=(Label&&) = delete;

		void Init() override;

		// Single line label at position 'rect'
		static LabelPtr CreateSingle(const char* id, RendererRef renderer, Rect rect, const char* label, FontRef font = nullptr, TextAlign align = TEXT_SINGLE_DEFAULT);

		// Fills whole parent window
		static LabelPtr CreateFill(const char* id, RendererRef renderer, const char* label, FontRef font = nullptr, TextAlign align = TEXT_FILL_DEFAULT);

		// Auto-size
		static LabelPtr CreateAutoSize(const char* id, RendererRef renderer, Rect rect, const char* label, FontRef font = nullptr, TextAlign align = TEXT_AUTOSIZE_DEFAULT);

		void SetForegroundColor(Color color) override;

		Rect GetRect(bool relative = true, bool scrolled = true) const override;
		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;

		void Draw() override;
		void Draw(const RectRef rect);

		void SetText(const char *) override;

		void SetAlign(uint8_t align);
		uint8_t GetAlign() { return m_labelAlign;  }

	protected:
		Label(const char* id, RendererRef renderer, Rect rect, const char* label, FontRef font, TextAlign align, CreationFlags flags);

		void DrawBackground(const GUI::RectRef &rect);
		Rect DrawFrame(const GUI::RectRef &rect);
		void RenderLabel();
		void DrawLabel(RectRef rect);

		TexturePtr m_labelText;
		Rect m_labelRect;
		TextAlign m_labelAlign;

		struct shared_enabler;
	};
}