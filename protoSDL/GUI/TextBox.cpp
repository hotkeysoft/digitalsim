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
	Uint32 TextBox::m_blinkTimerID = (Uint32)-1;

	TextBox::TextBox(const char * id, RendererRef renderer, Rect rect, const char * text) :
		Widget(id, renderer, nullptr, rect, text, nullptr, RES().FindFont("default")), m_blink(true)
	{
		m_backgroundColor = Color::C_WHITE;
		m_borderWidth = 0;
		m_margin = 5;
	}

	void TextBox::Init()
	{
		RenderText();
		if (m_blinkTimerID == (Uint32)-1)
		{
			m_blinkTimerID = WINMGR().AddTimer(530);
		}
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

		DrawBackground(&m_parent->GetClientRect(false, false));

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

		DrawCursor(&drawRect);
		DrawText(&drawRect);

	}

	void TextBox::DrawCursor(RectRef rect)
	{
		int yPos = rect->y + m_caretPos.y;
		int width = std::max(rect->w, m_rect.w + m_padding + m_borderWidth + m_margin);
		Rect outlineBox = { rect->x, yPos, width, m_lineHeight };
		DrawRect(&outlineBox, Color::C_VLIGHT_GREY);

		if (m_blink && (WINMGR().GetActive() == m_parent))
		{
			int xPos = rect->x + m_caretPos.x;
			SetDrawColor(Color::C_DARK_GREY);
			SDL_RenderDrawLine(m_renderer, xPos, yPos, xPos, yPos + m_lineHeight);
		}
	}

	void TextBox::DrawText(RectRef rect)
	{
		for (size_t i = 0; i<m_lines.size(); ++i)
		{
			auto & line = m_lines[i];

			Rect target = { rect->x, rect->y + ((int)i*m_lineHeight), line.rect.w, line.rect.h };
			SDL_RenderCopy(m_renderer, line.texture.get(), &line.rect, &target);
		}
	}

	void TextBox::DrawBackground(const RectRef &rect)
	{
		Rect fillRect = m_rect;
		fillRect.h = std::max(rect->h, m_rect.h);
		fillRect.w = std::max(rect->w, m_rect.w + m_margin + m_borderWidth + m_padding);

		DrawFilledRect(rect, m_backgroundColor);
//		DrawFilledRect(&fillRect.Offset(&rect->Origin()), m_backgroundColor);
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
				SDL_Surface* surface = TTF_RenderText_Blended(m_font, line.text.c_str(), m_foregroundColor);
				line.texture = SurfaceToTexture(surface);
				SDL_QueryTexture(line.texture.get(), NULL, NULL, &line.rect.w, &line.rect.h);
			}

			maxWidth = std::max(maxWidth, line.rect.w);
		}
	
		m_lineHeight = TTF_FontLineSkip(m_font);
		m_rect = { 0, 0, maxWidth, (int)m_lines.size() * TTF_FontLineSkip(m_font) };
	}

	void TextBox::InsertLine(const char * text, size_t at)
	{
		if (at > m_lines.size() - 1)
		{
			m_lines.push_back(text);
		}
		else
		{
			m_lines.insert(m_lines.begin() + at, text);
		}
		RenderLines();
	}

	void TextBox::InsertAt(const char * text, size_t line, size_t col)
	{
		if (text == nullptr)
			return;

		if (m_lines.size() == 0)
		{
			InsertLine(text);
		}
		else
		{
			line = std::min(m_lines.size() - 1, line);
			std::string s = m_lines[line].text;
			col = std::min(s.length(), col);
			s.insert(col, text);
			m_lines[line] = s;
			RenderLines();
		}
	}

	void TextBox::Insert(const char * text)
	{
		if (text)
		{
			InsertAt(text, m_currentPos.y, m_currentPos.x);
			MoveCursorRel((int16_t)strlen(text), 0);
		}
	}

	void TextBox::DeleteLine(size_t line)
	{
		line = std::min(m_lines.size() - 1, line);
		m_lines.erase(m_lines.begin() + line);
	}

	void TextBox::MoveCursor(int x, int y)
	{
		m_currentPos.x = std::max(0, x);
		m_currentPos.y = std::max(0, y);

		m_currentPos.y = std::min((int)m_lines.size() - 1, m_currentPos.y);
		m_currentPos.x = std::min((int)m_lines[m_currentPos.y].text.length(), m_currentPos.x);

		m_caretPos.y = m_currentPos.y * m_lineHeight;	
		if (m_currentPos.x == 0)
		{
			m_caretPos.x = 0;
		}
		else
		{
			std::string subStr = m_lines[m_currentPos.y].text.substr(0, m_currentPos.x);
			TTF_SizeText(m_font, subStr.c_str(), &m_caretPos.x, nullptr);
		}
	}

	void TextBox::MoveCursorRel(int16_t deltaX, int16_t deltaY)
	{
		MoveCursor(m_currentPos.x + deltaX, m_currentPos.y + deltaY);
	}

	void TextBox::Return()
	{	
		const std::string &toSplit = m_lines[m_currentPos.y].text;
		std::string endPart = toSplit.substr(m_currentPos.x);
		m_lines[m_currentPos.y] = toSplit.substr(0, m_currentPos.x);
		InsertLine(endPart.c_str(), m_currentPos.y+1); // RenderLines();
		MoveCursorRel(INT16_MIN, 1);
	}

	Point TextBox::CursorAt(PointRef pt)
	{
		Point cursorPos(-1, -1);

		Rect client = m_parent->GetClientRect(false, true).Deflate(m_margin + m_borderWidth + m_padding);
				
		Point rel(pt->x - client.x, pt->y - client.y);

		cursorPos.y = (rel.y / m_lineHeight);
		std::cout << "CursorAt " << rel << ", computed: " << cursorPos << std::endl;

		auto & line = m_lines[cursorPos.y];

		if (rel.x < 0)
		{
			cursorPos.x = INT_MIN;
		}
		else if (rel.x > line.rect.w)
		{
			cursorPos.x = INT_MAX;
		}
		else
		{

		}

		std::cout << "CursorAt " << rel << ", computed: " << cursorPos << std::endl;

		MoveCursor(cursorPos.x, cursorPos.y);
		return cursorPos;
	}

	void TextBox::Backspace()
	{
		if (m_currentPos.x == 0 && m_currentPos.y == 0)
			return;

		// Wrap line
		if (m_currentPos.x == 0)
		{
			TextLine &prevLine = m_lines[m_currentPos.y - 1];
			TextLine &currLine = m_lines[m_currentPos.y];
			size_t prevX = prevLine.text.size();

			prevLine = prevLine.text + currLine.text;
			DeleteLine(m_currentPos.y);
			MoveCursor((int)prevX, m_currentPos.y - 1);
		}
		else // Normal case
		{
			m_lines[m_currentPos.y].text.erase(m_currentPos.x-1, 1);
			m_lines[m_currentPos.y].texture = nullptr;
			MoveCursorRel(-1, 0);
		}
		RenderLines();
	}

	void TextBox::Delete()
	{
		TextLine &currLine = m_lines[m_currentPos.y];
		if (m_currentPos.y == m_lines.size() - 1 && m_currentPos.x == currLine.text.size())
			return;

		// Wrap line
		if (m_currentPos.x == currLine.text.size())
		{
			TextLine &nextLine = m_lines[m_currentPos.y + 1];
			TextLine &currLine = m_lines[m_currentPos.y];

			currLine = currLine.text + nextLine.text;
			DeleteLine(m_currentPos.y + 1);
		}
		else // Normal case
		{
			m_lines[m_currentPos.y].text.erase(m_currentPos.x, 1);
			m_lines[m_currentPos.y].texture = nullptr;
		}

		RenderLines();
	}
	
	bool TextBox::HandleEvent(SDL_Event *e)
	{
		Point pt(e->button.x, e->button.y);
//		HitResult hit = HitTest(&pt);
		switch (e->type)
		{
		case SDL_USEREVENT:
		{
			if (e->user.code == m_blinkTimerID)
			{
				m_blink = !m_blink;
			}
			return true;
		}
		case SDL_TEXTINPUT:
			Insert(e->text.text);
			break;
		case SDL_KEYDOWN:
		{
			bool ctrl = SDL_GetModState() & KMOD_CTRL;
			switch (e->key.keysym.sym)
			{
			case SDLK_LEFT:
				MoveCursorRel(-1, 0);
				break;
			case SDLK_RIGHT:
				MoveCursorRel(1, 0);
				break;
			case SDLK_UP:
				MoveCursorRel(0, -1);
				break;
			case SDLK_DOWN:
				MoveCursorRel(0, 1);
				break;
			case SDLK_HOME:
				MoveCursorRel(INT16_MIN, ctrl ? INT16_MIN : 0);
				break;
			case SDLK_END:
				MoveCursorRel(INT16_MAX, ctrl ? INT16_MAX : 0);
				break;
			case SDLK_BACKSPACE:
				Backspace();
				break;
			case SDLK_DELETE:
				Delete();
				break;
			case SDLK_RETURN:
				Return();
				break;
			default:
				return false;
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			Point cursor = CursorAt(&pt);
			return cursor.x >= 0;
		}
		default:
			return false;
		}

		return true;
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