#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "TextBox.h"
#include <algorithm>
#include <sstream>
#include <cassert>

namespace GUI
{
	Uint32 TextBox::m_blinkTimerID = (Uint32)-1;

	TextBox::TextBox(const char * id, RendererRef renderer, Rect rect, const char * text, bool fill) :
		Widget(id, renderer, nullptr, rect, text, nullptr, RES().FindFont("mono")), m_blink(true), m_fill(fill), m_xOffset(0), m_lineWidth(-1)
	{
		m_backgroundColor = Color::C_WHITE;
		m_borderColor = Color::C_BLACK;
		m_showBorder = fill ? false : true;
		m_borderWidth = fill ? 0 : 1;
		m_margin = m_fill ? 5 : 0;
		m_padding = m_fill ? 0 : 2;
	}

	void TextBox::Init()
	{
		m_lineHeight = TTF_FontLineSkip(m_font);
		if (TTF_FontFaceIsFixedWidth(m_font))
		{
			TTF_SizeText(m_font, "X", &m_charWidth, nullptr);
		}
		else
		{
			m_charWidth = 0;
		}

		RenderText();
		if (m_blinkTimerID == (Uint32)-1)
		{
			m_blinkTimerID = WINMGR().AddTimer(530);
		}
	}

	TextBoxPtr TextBox::CreateSingleLine(const char * id, RendererRef renderer, Rect rect, const char * text)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, text, false);
		return std::static_pointer_cast<TextBox>(ptr);
	}
	
	TextBoxPtr TextBox::CreateFill(const char * id, RendererRef renderer, const char * text)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, Rect(), text, true);
		return std::static_pointer_cast<TextBox>(ptr);
	}


	Rect TextBox::GetRect(bool relative, bool scrolled) const
	{
		Rect parent = m_parent->GetClientRect(relative, scrolled);

		return m_rect.Offset(&parent.Origin());
	}

	Rect TextBox::GetClientRect(bool relative, bool scrolled) const
	{
		return GetRect(relative, scrolled);
	}

	void TextBox::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect drawRect;
		if (m_fill)
		{
			DrawBackground(&m_parent->GetClientRect(false, false));
			drawRect = m_parent->GetClientRect(false, true);
		}
		else
		{			
			drawRect = GetRect(false, true);
		}

		if (m_margin)
		{
			drawRect = drawRect.Deflate(m_margin);
		}

		if (!m_fill && !m_backgroundColor.IsTransparent())
		{
			DrawFilledRect(&drawRect, m_backgroundColor);
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

		if (IsFocused())
		{
			DrawCursor(&drawRect);
		}

		DrawText(&drawRect);
	}

	int TextBox::GetShrinkFactor()
	{
		return m_padding + m_margin + (m_showBorder ? m_borderWidth : 0);
	}

	void TextBox::DrawCursor(RectRef rect)
	{
		int yPos = rect->y + m_caretPos.y;
	
		if (m_fill)
		{
			int width = std::max(rect->w, m_rect.w);
			Rect outlineBox = { rect->x, yPos, width, m_lineHeight };
			DrawRect(&outlineBox, Color::C_VLIGHT_GREY);
		}

		if (m_blink && (WINMGR().GetActive() == m_parent))
		{
			int xPos = rect->x + m_caretPos.x - m_xOffset;
			SetDrawColor(Color::C_DARK_GREY);
			SDL_RenderDrawLine(m_renderer, xPos, yPos, xPos, yPos + m_lineHeight);
			SDL_RenderDrawLine(m_renderer, xPos+1, yPos, xPos+1, yPos + m_lineHeight);
		}
	}

	void TextBox::DrawText(RectRef rect)
	{
		for (size_t i = 0; i<m_lines.size(); ++i)
		{
			auto & line = m_lines[i];

			Rect target = { rect->x, rect->y + ((int)i*m_lineHeight), line.rect.w, line.rect.h };

			Rect source = line.rect;
			
			if (!m_fill)
			{
				target.w = std::min(rect->w, source.w);
				source.x += m_xOffset;
				source.w = target.w;
			}

			SDL_RenderCopy(m_renderer, line.texture.get(), &source, &target);
		}
	}

	void TextBox::DrawBackground(const RectRef &rect)
	{
		Rect fillRect = m_rect;
		fillRect.h = std::max(rect->h, m_rect.h);
		fillRect.w = std::max(rect->w, m_rect.w);

		DrawFilledRect(rect, m_backgroundColor);
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
			if (!line.texture && !line.text.empty())
			{
				SDL_Surface* surface = TTF_RenderText_Blended(m_font, line.text.c_str(), m_foregroundColor);
				line.texture = SurfaceToTexture(surface);
				SDL_QueryTexture(line.texture.get(), NULL, NULL, &line.rect.w, &line.rect.h);
			}

			maxWidth = std::max(maxWidth, line.rect.w);
		}
		
		if (m_fill)
		{
			Rect newRect = { 0, 0, maxWidth + (2 * GetShrinkFactor()), (int)m_lines.size() * TTF_FontLineSkip(m_font) + (2 * GetShrinkFactor()) };
			if (!newRect.IsEqual(&m_rect))
			{
				m_rect = newRect;
				GetParentWnd()->GetScrollBars()->RefreshScrollBarStatus();
			}
		}
		else // Single line mode
		{
			if (m_lineWidth == -1)
			{
				m_lineWidth = maxWidth;
				m_xOffset = 0;
			}
			else
			{
				int delta =  maxWidth - m_lineWidth;
				if (delta < 0)
				{
					m_xOffset = std::max(0, m_xOffset + delta);
				}

				m_lineWidth = maxWidth;
			}
		}
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

		ScrollCursorIntoView();
	}

	void TextBox::MoveCursorRel(int16_t deltaX, int16_t deltaY)
	{
		MoveCursor(m_currentPos.x + deltaX, m_currentPos.y + deltaY);
	}

	void TextBox::Return()	
	{	
		if (m_fill)
		{
			const std::string &toSplit = m_lines[m_currentPos.y].text;
			std::string endPart = toSplit.substr(m_currentPos.x);
			m_lines[m_currentPos.y] = toSplit.substr(0, m_currentPos.x);
			InsertLine(endPart.c_str(), m_currentPos.y + 1); // RenderLines();
			MoveCursorRel(INT16_MIN, 1);
		}
	}

	// For proportial fonts
	int TextBox::FindStringPos(const std::string & in, int pos, int len, int fraction)
	{		
		if (len <= 0)
		{
			return 0;
		}

		std::string subStr = in.substr(0, len);

		int width;
		TTF_SizeText(m_font, subStr.c_str(), &width, nullptr);

		if (fraction == 1)
		{
			return (pos < width) ? len - 1 : len;
		}

		if (pos < width)
		{
			len -= (fraction+1) / 2;
			return FindStringPos(in, pos, len, (fraction+1)/2);
		}
		else 
		{
			len += (fraction+1) / 2;
			return FindStringPos(in, pos, len, (fraction+1)/2);
		}
	}

	void TextBox::ScrollCursorIntoView()
	{	
		if (m_fill)
		{
			WindowRef parentWnd = GetParentWnd();
			assert(parentWnd); // TODO update for widget in widget
			Rect rect = m_parent->GetClientRect(true, true).Deflate(GetShrinkFactor());

			int deltaX = m_caretPos.x + rect.x;
			if (deltaX < 0)
			{				
				parentWnd->GetScrollBars()->ScrollRel(&Point(deltaX - GetShrinkFactor(), 0));
			}

			// TODO: m_charWidth not set for proportional fonts
			deltaX = (m_caretPos.x + rect.x + m_charWidth) - rect.w;
			if (deltaX > 0)
			{
				parentWnd->GetScrollBars()->ScrollRel(&Point(deltaX, 0));
			}

			int deltaY = m_caretPos.y + rect.y;
			if (deltaY < 0)
			{
				parentWnd->GetScrollBars()->ScrollRel(&Point(0, deltaY - GetShrinkFactor()));
			}
			
			deltaY = (m_caretPos.y+m_lineHeight + rect.y) - rect.h;
			if (deltaY > 0)
			{
				parentWnd->GetScrollBars()->ScrollRel(&Point(0, deltaY));
			}
		}
		else
		{
			Rect rect = GetClientRect(true, false).Deflate(GetShrinkFactor());
			int deltaX = m_caretPos.x - m_xOffset;
			if (deltaX < 0)
			{		
				ScrollX(rect.w, deltaX);
			}

			// TODO: m_charWidth not set for proportional fonts
			deltaX = (m_caretPos.x + m_charWidth - m_xOffset) - rect.w;
			if (deltaX > 0)
			{
				ScrollX(rect.w, deltaX);
			}
		}
	}

	// Single line mode
	int TextBox::GetLineWidth()
	{
		return m_lineWidth;
	}

	void TextBox::ScrollX(int fieldWidth, int16_t offset)
	{
		if (offset > 0)
		{
			m_xOffset += offset;

			int lineWidth = GetLineWidth();
			if (m_xOffset + fieldWidth > lineWidth)
			{
				m_xOffset = std::max(0, lineWidth - fieldWidth);
			}
		}
		else if (offset < 0)
		{
			m_xOffset = std::max(0, m_xOffset + offset);
		}
	}

	Point TextBox::CursorAt(PointRef pt)
	{
		Point cursorPos(-1, -1);

		Rect client;
		if (m_fill)
		{
			client = m_parent->GetClientRect(false, true).Deflate(GetShrinkFactor());
		}
		else
		{
			client = GetRect(false, true).Deflate(GetShrinkFactor());
		}

		Point rel(pt->x + m_xOffset - client.x, pt->y - client.y);

		cursorPos.y = (rel.y / m_lineHeight);
		if (cursorPos.y > m_lines.size() - 1)
		{
			return Point(0, INT_MAX);
		}

		auto & line = m_lines[cursorPos.y];

		if (rel.x < 0)
		{
			cursorPos.x = INT_MIN;
		}
		else if (rel.x > line.rect.w)
		{
			cursorPos.x = INT_MAX;
		}
		else if (line.text.size() == 1)
		{
			cursorPos.x = 0;
		}
		else if (m_charWidth > 0)
		{
			// Fixed fonts
			cursorPos.x = (rel.x+1) / m_charWidth;
		}
		else
		{
			// Proportial fonts
			cursorPos.x = FindStringPos(line.text, rel.x, (int)line.text.size(), (int)line.text.size());
		}

		MoveCursor(cursorPos.x, cursorPos.y);
		return cursorPos;
	}

	void TextBox::Backspace()
	{
		if (m_currentPos.x == 0 && m_currentPos.y == 0)
			return;

		// Wrap line
		if (m_fill && m_currentPos.x == 0)
		{
			TextLine &prevLine = m_lines[m_currentPos.y - 1];
			TextLine &currLine = m_lines[m_currentPos.y];
			size_t prevX = prevLine.text.size();

			prevLine = prevLine.text + currLine.text;
			DeleteLine(m_currentPos.y);
			RenderLines();
			MoveCursor((int)prevX, m_currentPos.y - 1);
		}
		else // Normal case
		{
			m_lines[m_currentPos.y].text.erase(m_currentPos.x-1, 1);
			m_lines[m_currentPos.y].texture = nullptr;
			RenderLines();
			MoveCursorRel(-1, 0);
		}		
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
	
	void TextBox::MovePage(int16_t deltaY)
	{
		if (m_fill)
		{			
			Rect client = m_parent->GetClientRect(true, false);
			int linesPerPage = client.h / m_lineHeight;

			MoveCursorRel(0, deltaY * linesPerPage);
		}
	}

	HitResult TextBox::HitTest(const PointRef pt)
	{
		Rect parent = m_parent->GetClientRect(false, true);
		if (m_fill && parent.PointInRect(pt))
		{
			return HitResult(HitZone::HIT_CONTROL, this);
		}
		else if (!m_fill && m_rect.Offset(&parent.Origin()).PointInRect(pt))
		{
			return HitResult(HitZone::HIT_CONTROL, this);
		}

		return HitZone::HIT_NOTHING;
	}

	bool TextBox::HandleEvent(SDL_Event *e)
	{
		Point pt(e->button.x, e->button.y);
		HitResult hit = HitTest(&pt);
		switch (e->type)
		{
		case SDL_USEREVENT:
		{
			if (e->user.code == m_blinkTimerID && IsFocused())
			{
				m_blink = !m_blink;
			}
			return false;
		}
		case SDL_MOUSEMOTION:	
			if (hit)
			{
				SDL_SetCursor(RES().FindCursor("edit.ibeam"));
			}
			break;
		case SDL_TEXTINPUT:
			if (IsFocused())
			{
				Insert(e->text.text);
			}
			else
			{
				return false;
			}
			break;
		case SDL_KEYDOWN:
		{
			if (IsFocused())
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
				case SDLK_PAGEDOWN:
					MovePage(1);
					break;
				case SDLK_PAGEUP:
					MovePage(-1);
					break;
				default:
					return false;
				}
			}
			else
			{
				return false;
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			Point cursor = CursorAt(&pt);
			SetActive();
			SetFocus(this);
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