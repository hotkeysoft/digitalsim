#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "WindowManager.h"
#include <string>
#include <vector>

namespace GUI
{
	class TextBox : public Widget
	{
	public:
		virtual ~TextBox() = default;
		TextBox(const TextBox&) = delete;
		TextBox& operator=(const TextBox&) = delete;
		TextBox(TextBox&&) = delete;
		TextBox& operator=(TextBox&&) = delete;

		void Init() override;

		static TextBoxPtr Create(const char* id, RendererRef renderer, Rect rect, const char* text);

		void Draw() override;

		void SetText(const char *) override;

		void InsertAt(const char * text, size_t line = std::numeric_limits<size_t>::max(), size_t col = std::numeric_limits<size_t>::max());
		void InsertLine(const char * text = nullptr, size_t at = std::numeric_limits<size_t>::max());

	protected:
		struct TextLine
		{
			TextLine() = default;
			TextLine(const char * str) : text(str ? str : "") {}
			TextLine(std::string str) : text(str) {}
			const bool operator==(const std::string& rhs) const { return text == rhs; }
			const bool operator!=(const std::string& rhs) const { return text != rhs; }

			std::string text;
			TexturePtr texture;
			Rect rect;
		};
		using TextLines = std::vector<TextLine>;

		TextBox(const char* id, RendererRef renderer, Rect rect, const char* text);

		void RenderText();
		void SplitLines();
		void RenderLines();
		void DrawText(RectRef rect);

		TextLines m_lines;
		int m_lineHeight;
		Rect m_textRect;

		struct shared_enabler;
	};
}