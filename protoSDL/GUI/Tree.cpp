#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Tree.h"
#include <algorithm>

namespace GUI
{
	TreeNode::TreeNode(const char* label, ImagePtr image, TreeNodeRef parent, TreeRef tree) : 
		m_image(image), m_label(label), m_parent(parent), m_tree(tree), m_depth(0), m_open(true)
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
		if (!m_texture && !m_label.empty())
		{
			SDL_Surface* surface = TTF_RenderText_Blended(m_tree->GetFont(), m_label.c_str(), m_tree->GetForegroundColor());
			m_texture = WINMGR().SurfaceToTexture(surface);
			SDL_QueryTexture(m_texture.get(), NULL, NULL, &m_rect.w, &m_rect.h);
		}
	}

	Tree::Tree(const char * id, RendererRef renderer, FontRef font, bool fill) :
		Widget(id, renderer, nullptr, Rect(), nullptr, nullptr, font), m_fill(fill), m_lineHeight(20), m_charWidth(0)
	{
		m_backgroundColor = Color::C_WHITE;
		m_margin = 5;
	}

	void Tree::Init()
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

		RenderNodes();
	}

	TreePtr Tree::Create(const char * id, RendererRef renderer, FontRef font)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, font, true);
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

	void Tree::Draw()
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
		Rect source = node->m_rect;

		Rect& target = node->m_drawRect;
		target = { rect->x, rect->y + (line*m_lineHeight), node->m_rect.w, node->m_rect.h };
				
		SetDrawColor(Color::C_DARK_GREY);
		for (int i=0; i<node->m_depth; ++i)
		{
			int x = target.x + (m_lineHeight * i) + (m_lineHeight / 2);
			SDL_RenderDrawLine(m_renderer, x, target.y, x, target.y + m_lineHeight);
		}
		target.x += (m_lineHeight * node->m_depth);

		SDL_RenderCopy(m_renderer, node->m_texture.get(), &source, &target);	
	}

	void Tree::RenderNodes()
	{
		int maxWidth = 0;
		for (auto node : m_nodes)
		{
			node->Render();

			// TODO: Include everything that will be rendered (image, lines, widgets, etc.)
			int width = node->m_rect.w + (m_lineHeight * node->m_depth);
			maxWidth = std::max(width, maxWidth);

			std::cout << "Node: [" << node->GetLabel() << "], hasChild,PrevSib,NextSib:"
				<< NodeHasChildren(node)
				<< NodeHasPreviousSibling(node)
				<< NodeHasNextSibling(node) << std::endl;

		}

		if (m_fill)
		{
			Rect newRect = { 0, 0, 
				maxWidth + (2 * GetShrinkFactor()), 
				GetVisibleLineCount() * TTF_FontLineSkip(m_font) + (2 * GetShrinkFactor()) };

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
			return false;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
//			Point cursor = CursorAt(&pt);
			SetActive();
			SetFocus(this);
//			return cursor.x >= 0;
			return true;
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
	
	void Tree::OpenNode(TreeNodeRef node, bool open)
	{
		if (node == nullptr)
		{
			throw std::invalid_argument("Node is null");
		}

		// For efficiency, assume that node belongs to m_nodes...
		node->m_open = open;
		RenderNodes();
	}

	TreeNodeRef Tree::AddRootNode(const char * label, ImagePtr image)
	{
		if (!m_nodes.empty())
		{
			throw std::invalid_argument("tree already has node");
		}

		TreeNodeRef root = new TreeNode(label, image, nullptr, this);
		m_nodes.push_back(root);
		return root;
	}

	TreeNodeRef Tree::AddNode(const char * label, ImagePtr image, TreeNodeRef parent)
	{
		if (label == nullptr)
		{
			throw std::invalid_argument("label is null");
		}

		if (parent == nullptr)
		{
			return AddRootNode(label, image);
		}

		auto iter = FindNode(parent);
		if (iter == m_nodes.end())
		{
			throw std::invalid_argument("parent node not found");
		}
		while (++iter != m_nodes.end() && (*iter)->GetParent() == parent);

		TreeNodeRef node = new TreeNode(label, image, parent, this);
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

	struct Tree::shared_enabler : public Tree
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Tree(std::forward<Args>(args)...)
		{
		}
	};
}