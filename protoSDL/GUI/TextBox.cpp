#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "TextBox.h"
#include <algorithm>
#include <sstream>

namespace GUI
{
	TextBox::TextBox(const char * id, RendererRef renderer, Rect rect, const char * text) :
		Widget(id, renderer, nullptr, rect, text, nullptr, RES().FindFont("mono"))
	{
		m_backgroundColor = Color::C_WHITE;
		m_borderWidth = 1;
	}

	void TextBox::Init()
	{
		RenderText();
	}

	TextBoxPtr TextBox::Create(const char * id, RendererRef renderer, Rect rect, const char * text)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, text);
		return std::static_pointer_cast<TextBox>(ptr);
	}

	void TextBox::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect drawRect = m_parent->GetClientRect(false, true);

		if (m_margin)
		{
			drawRect = drawRect.Deflate(m_margin);
		}

		if (!m_backgroundColor.IsTransparent())
		{
			DrawRect(&drawRect, m_backgroundColor);
		}

		if (m_showBorder)
		{
			for (int i = 0; i < m_borderWidth; ++i)
			{
				DrawRect(&drawRect, m_borderColor);
				drawRect = drawRect.Deflate(1);
			}
		}

		if (m_padding)
		{
			drawRect = drawRect.Deflate(m_padding);
		}

		DrawText(&drawRect);
	}

	void TextBox::DrawText(RectRef rect)
	{
		Rect fillRect = m_rect;
		fillRect.h = std::max(rect->h, m_rect.h);
		fillRect.w = std::max(rect->w, m_rect.w);
		
		DrawFilledRect(&fillRect.Offset(&rect->Origin()), m_backgroundColor);
		for (size_t i = 0; i<m_lines.size(); ++i)
		{
			auto & line = m_lines[i];

			Rect target = { rect->x, rect->y + ((int)i*m_lineHeight), line.rect.w, line.rect.h };
			SDL_RenderCopy(m_renderer, line.texture.get(), &line.rect, &target);
		}
	}

	void TextBox::SetText(const char * text)
	{
		Widget::SetText(text);
		RenderText();
	}

	void TextBox::RenderText()
	{
		SplitLines();
		RenderLines();
	}

	void TextBox::SplitLines()
	{
		std::stringstream ss(m_text);
		int currLine = 0;
		while (ss.good())
		{
			std::string line;
			std::getline(ss, line);

			if (m_lines.size() < currLine + 1)
			{
				m_lines.push_back(line);
			}
			else // Act only if line changed
			{
				if (m_lines[currLine] != line)
				{
					m_lines[currLine] = line;
				}
			}
			++currLine;
		}

		if (currLine < m_lines.size())
		{
			m_lines.resize(currLine);
		}
		std::cout << "TextBox: " << currLine << " lines" << std::endl;
	}

	void TextBox::RenderLines()
	{
		int maxWidth = 0;
		if (m_font == nullptr)
		{
			throw std::invalid_argument("TextBox: No Font");
		}

		for (auto & line : m_lines)
		{
			if (!line.texture)
			{
				std::cout << "Render: " << line.text << std::endl;
				SDL_Surface* surface = TTF_RenderText_Blended(m_font, line.text.c_str(), m_foregroundColor);
				line.texture = SurfaceToTexture(surface);
				SDL_QueryTexture(line.texture.get(), NULL, NULL, &line.rect.w, &line.rect.h);
			}

			maxWidth = std::max(maxWidth, line.rect.w);
		}
	
		m_lineHeight = TTF_FontLineSkip(m_font);
		m_rect = { 0, 0, maxWidth, (int)m_lines.size() * TTF_FontLineSkip(m_font) };
		std::cout << "font line skip: " << m_lineHeight << ", rect = " << m_rect << std::endl;
	}

	struct TextBox::shared_enabler : public TextBox
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: TextBox(std::forward<Args>(args)...)
		{
		}
	};
}