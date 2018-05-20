#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Label.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Tree.h"
#include <algorithm>
#include <cassert>

namespace GUI
{
	TreeNode::TreeNode(RendererRef renderer, const char* text, ImageRef opened, ImageRef closed, TreeNodeRef parent, TreeRef tree) :
		m_renderer(renderer), m_text(text), m_openedImage(opened), m_closedImage(closed), m_parent(parent), m_tree(tree), m_depth(0), m_opened(true)
	{
		TreeNodeRef curr = parent;
		while (curr) 
		{
			++m_depth;
			curr = curr->m_parent;
		}
	}

	bool GUI::TreeNode::IsVisible() const
	{
		bool visible = true;
		TreeNodeRef parent = m_parent;
		while (parent)
		{
			if (!parent->IsOpen())
				return false;
			parent = parent->m_parent;
		}
		return true;
	}

	void TreeNode::Render()
	{
		if (!m_label && !m_text.empty())
		{
			SDL_Surface* surface = TTF_RenderText_Blended(m_tree->GetFont(), m_text.c_str(), m_tree->GetForegroundColor());

			m_label = Label::CreateAutoSize("l", m_renderer, Rect(), m_text.c_str());
			m_label->SetPadding(Dimension(5, 0));
			m_label->Init();
		}
	}

	Tree::Tree(const char* id, RendererRef renderer, bool fill, int lineHeight, FontRef font) :
		Widget(id, renderer, nullptr, Rect(), nullptr, nullptr, font), m_fill(fill), 
		m_lineHeight(clip(lineHeight, 8, 255)), 
		m_indent(clip(lineHeight, 0, 255))
	{
		m_backgroundColor = Color::C_WHITE;
		m_margin = 20;
		m_padding = 5;
		m_showBorder = true;
	}

	void Tree::Init()
	{
		if (m_lineHeight < 8)
		{
			throw std::invalid_argument("line height too small");
		}

		RenderNodes();
	}

	TreePtr Tree::Create(const char * id, RendererRef renderer, int lineHeight, FontRef font)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, true, lineHeight, font);
		return std::static_pointer_cast<Tree>(ptr);
	}
	
	Rect Tree::GetRect(bool relative, bool scrolled) const
	{
		Rect parent = m_parent->GetClientRect(relative, scrolled);

		return m_rect.Offset(&parent.Origin());
	}

	Rect Tree::GetClientRect(bool relative, bool scrolled) const
	{
		return GetRect(relative, scrolled);
	}

	void Tree::DrawFrame(const RectRef &rect)
	{
		Rect frameRect = *rect;

		if (m_fill)
		{
			DrawBackground(&m_parent->GetClientRect(false, false));
		}

		if (m_margin)
		{
			frameRect = frameRect.Deflate(m_margin);
		}

		if (!m_fill && !m_backgroundColor.IsTransparent())
		{
			DrawFilledRect(&frameRect, m_backgroundColor);
		}

		if (m_showBorder)
		{
			for (int i = 0; i < m_borderWidth; ++i)
			{
				DrawRect(&frameRect, m_borderColor);
				frameRect = frameRect.Deflate(1);
			}
		}

		SDL_RenderSetClipRect(m_renderer, &frameRect);
	}

	void Tree::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect drawRect;
		if (m_fill)
		{
			DrawFrame(&m_parent->GetClientRect(false, false));
			drawRect = m_parent->GetClientRect(false, true);
		}
		else
		{
			drawRect = GetRect(false, true);
			
			DrawFrame(&drawRect);
		}

		drawRect = drawRect.Deflate(GetShrinkFactor());

		DrawTree(&drawRect);
	}

	void Tree::DrawBackground(const RectRef &rect)
	{
		Rect fillRect = m_rect;
		fillRect.h = std::max(rect->h, m_rect.h);
		fillRect.w = std::max(rect->w, m_rect.w);

		DrawFilledRect(rect, m_backgroundColor);
	}

	void Tree::DrawTree(const RectRef & rect)
	{
		int line = 0;
		for (auto node : m_nodes)
		{
			if (node->IsVisible())
			{
				DrawNode(rect, line, node);
				++line;
			}
		}
	}
	
	void Tree::DrawNode(const RectRef &rect, int line, TreeNodeRef node)
	{
		Rect target = *rect;
		target.x += (m_indent * node->m_depth);
		target.y += (line * m_lineHeight);
		target.w = node->m_label->GetRect().w;
		target.h = m_lineHeight;

		ImageRef image = (node->m_opened && NodeHasChildren(node)) ? node->m_openedImage : node->m_closedImage;

		if (image)
		{
			Rect imageRect(target.x, target.y, m_lineHeight, m_lineHeight);
			image->Draw(&imageRect, Image::IMG_H_CENTER | Image::IMG_V_CENTER);
			target.x += m_lineHeight;
		}

		if (node->m_label)
		{
			node->m_label->SetBackgroundColor(node->IsSelected() ? m_selectedBgColor : m_backgroundColor);
			node->m_label->SetForegroundColor(node->IsSelected() ? m_selectedFgColor : m_foregroundColor);
			node->m_label->Draw(&target);
			node->m_labelRect = target;
		}
	}

	void Tree::RenderNodes()
	{
		int maxWidth = 0;
		for (auto node : m_nodes)
		{
			node->Render();

			// TODO: Include everything that will be rendered (image, lines, widgets, etc.)
			if (node->IsVisible())
			{
				int width = node->m_label->GetRect().w + (m_indent * node->m_depth + (node->m_openedImage ? m_lineHeight : 0));
				maxWidth = std::max(width, maxWidth);
			}
		}

		if (m_fill)
		{
			Rect newRect = { 0, 0, 
				maxWidth + (2 * GetShrinkFactor().w), 
				GetVisibleLineCount() * m_lineHeight + (2 * GetShrinkFactor().h) };

			if (!newRect.IsEqual(&m_rect))
			{
				m_rect = newRect;
				if (m_parent)
				{
					GetParentWnd()->GetScrollBars()->RefreshScrollBarStatus();
				}
			}
		}
	}

	int Tree::GetVisibleLineCount()
	{
		return (int)std::count_if(m_nodes.begin(), m_nodes.end(), [](TreeNodeRef node) {return node->IsVisible(); });
	}

	bool Tree::HandleEvent(SDL_Event * e)
	{
		Point pt(e->button.x, e->button.y);
		HitResult hit = HitTest(&pt);
		switch (e->type)
		{
		case SDL_MOUSEMOTION:
			if (hit)
			{
				SDL_SetCursor(RES().FindCursor("default"));
			}
			break;
		case SDL_KEYDOWN:
		{
			if (IsFocused())
			{
				//bool ctrl = SDL_GetModState() & KMOD_CTRL;
				switch (e->key.keysym.sym)
				{
				case SDLK_LEFT:
					CloseSelection();
					break;
				case SDLK_RIGHT:
					OpenSelection();
					break;
				case SDLK_UP:
					MoveSelectionRel(-1);
					break;
				case SDLK_DOWN:
					MoveSelectionRel(1);
					break;
				case SDLK_HOME:
					MoveSelectionRel(INT16_MIN);
					break;
				case SDLK_END:
					MoveSelectionRel(INT16_MAX);
					break;
				case SDLK_PAGEDOWN:
					MoveSelectionPage(1);
					break;
				case SDLK_PAGEUP:
					MoveSelectionPage(-1);
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
			TreeNodeRef node = NodeAt(&pt);

			SetActive();
			SetFocus(this);

			if (node)
			{
				SelectNode(node);
			}

			return node != nullptr;
		}
		default:
			return false;
		}

		return true;
	}

	HitResult Tree::HitTest(const PointRef pt)
	{
		Rect parent = m_parent->GetClientRect(false, false);
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

	TreeNodeRef Tree::NodeAt(PointRef pt)
	{
		auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [pt](TreeNodeRef node) { return node->Hit(pt); });
		if (it != m_nodes.end())
		{
			return *it;
		}

		return nullptr;
	}
	
	void Tree::OpenNode(TreeNodeRef node, bool open)
	{
		if (node == nullptr)
		{
			throw std::invalid_argument("Node is null");
		}

		// For efficiency, assume that node belongs to m_nodes...
		node->m_opened = open;
		RenderNodes();
	}

	TreeNodeRef Tree::AddRootNode(const char * label, ImageRef opened, ImageRef closed)
	{
		if (!m_nodes.empty())
		{
			throw std::invalid_argument("tree already has node");
		}

		TreeNodeRef root = new TreeNode(m_renderer, label, opened, closed, nullptr, this);
		m_nodes.push_back(root);
		return root;
	}

	TreeNodeRef Tree::AddNode(const char * label, TreeNodeRef parent)
	{
		return AddNode(label, nullptr, nullptr, parent);
	}

	TreeNodeRef Tree::AddNode(const char * label, ImageRef image, TreeNodeRef parent)
	{
		return AddNode(label, image, image, parent);
	}

	TreeNodeRef Tree::AddNode(const char * label, ImageRef opened, ImageRef closed, TreeNodeRef parent)
	{
		if (label == nullptr)
		{
			throw std::invalid_argument("label is null");
		}

		if (parent == nullptr)
		{
			return AddRootNode(label, opened, closed);
		}

		auto iter = FindNode(parent);
		if (iter == m_nodes.end())
		{
			throw std::invalid_argument("parent node not found");
		}
		while (++iter != m_nodes.end() && (*iter)->GetParent() == parent);

		TreeNodeRef node = new TreeNode(m_renderer, label, opened, closed, parent, this);
		m_nodes.insert(iter, node);
		return node;
	}

	TreeNodeList::const_iterator GUI::Tree::FindNode(TreeNodeRef toFind) const
	{
		return std::find_if(m_nodes.begin(), m_nodes.end(), [toFind](TreeNodeRef node) {return toFind == node; });
	}

	bool GUI::Tree::NodeHasChildren(TreeNodeRef node)
	{
		auto it = FindNode(node);
		if (it == m_nodes.end())
		{
			throw std::invalid_argument("node not found");
		}

		++it;
		return (it != m_nodes.end() && (*it)->m_parent == node);
	}

	bool Tree::NodeHasNextSibling(TreeNodeRef node)
	{
		auto it = FindNode(node);
		if (it == m_nodes.end())
		{
			throw std::invalid_argument("node not found");
		}

		while (++it != m_nodes.end())
		{
			if ((*it)->m_parent == node->m_parent)
				return true;
		}
		return false;
	}

	bool Tree::NodeHasPreviousSibling(TreeNodeRef node)
	{
		auto it = FindNode(node);
		if (it == m_nodes.end())
		{
			throw std::invalid_argument("node not found");
		}
		
		while (it != m_nodes.begin())
		{
			--it;
			if ((*it)->m_parent == node->m_parent)
				return true;
		}
		return false;
	}

	void Tree::SelectNode(TreeNodeRef node)
	{
		for (auto node : m_nodes)
		{
			node->m_selected = false;
		}

		if (node)
		{
			node->m_selected = true;
		}
	}

	TreeNodeList::const_iterator Tree::FindSelectedNode() const
	{
		return std::find_if(m_nodes.begin(), m_nodes.end(), [](TreeNodeRef node) { return node->IsSelected(); });
	}

	TreeNodeRef Tree::GetSelectedNode() const
	{
		auto it = FindSelectedNode();
		if (it != m_nodes.end())
		{
			return *it;
		}

		return nullptr;
	}

	void Tree::MoveSelectionRel(int16_t deltaY)
	{
		auto it = FindSelectedNode();

		if (deltaY < 0 && it != m_nodes.begin())
		{
			while (deltaY < 0)
			{
				if (--it == m_nodes.begin())
					break;

				if ((*it)->IsVisible())
				{
					++deltaY;
				}
			}
		}
		else if (deltaY > 0 && it != m_nodes.end())
		{
			TreeNodeList::const_iterator lastVisible = m_nodes.end();
			while (deltaY > 0)
			{
				if (++it == m_nodes.end())
					break;

				if ((*it)->IsVisible())
				{
					lastVisible = it;
					--deltaY;
				}
			}

			it = lastVisible;
		}
		
		if (it != m_nodes.end())
		{
			SelectNode(*it);
			ScrollSelectionIntoView();
		}
	}
	void Tree::MoveSelectionPage(int16_t deltaY)
	{

	}

	void Tree::OpenSelection()
	{
		OpenNode(GetSelectedNode(), true);
	}

	void Tree::CloseSelection()
	{
		OpenNode(GetSelectedNode(), false);
	}

	void Tree::ScrollSelectionIntoView()
	{
		WindowRef parentWnd = GetParentWnd();
		assert(parentWnd); // TODO update for widget in widget

		Rect rectAbs = m_parent->GetClientRect(false, false).Deflate(GetShrinkFactor());
		PointRef scrollPos = parentWnd->GetScrollBars()->GetScrollPos();

		TreeNodeRef selected = GetSelectedNode();
		if (selected)
		{
			int deltaY = selected->m_labelRect.y - rectAbs.y + scrollPos->y;
			if (deltaY < 0)
			{
				parentWnd->GetScrollBars()->ScrollRel(&Point(0, deltaY - GetShrinkFactor().h));
			}

			deltaY = (selected->m_labelRect.y + m_lineHeight - rectAbs.y) - rectAbs.h + scrollPos->y;
			if (deltaY > 0)
			{
				parentWnd->GetScrollBars()->ScrollRel(&Point(0, deltaY));
			}
		}
	}

	struct Tree::shared_enabler : public Tree
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Tree(std::forward<Args>(args)...)
		{
		}
	};
}