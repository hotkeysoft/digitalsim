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
	TreeNode::TreeNode(const char* label, ImagePtr image, TreeNodeRef parent, TreeRef tree) : m_image(image), m_label(label), m_parent(parent), m_tree(tree), m_depth(0)
	{
		TreeNodeRef curr = parent;
		while (curr) 
		{
			++m_depth;
			curr = curr->m_parent;
		}
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
			DrawNode(rect, line, node);
			++line;
		}
	}
	
	void Tree::DrawNode(const RectRef &rect, int line, TreeNodeRef node)
	{
		Rect source = node->m_rect;
		Rect target = { rect->x, rect->y + (line*m_lineHeight), node->m_rect.w, node->m_rect.h };
				
		SetDrawColor(Color::C_DARK_GREY);
		for (int i=0; i<node->m_depth; ++i)
		{
			int x = target.x + (m_lineHeight * i) + (m_lineHeight / 2);
			SDL_RenderDrawLine(m_renderer, x, target.y, x, target.y + m_lineHeight);
		}
		target.x += (m_lineHeight * node->m_depth);

		//std::cout << "Node :" << node->GetLabel() << ", \tdepth = " << node->m_depth << std::endl;
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
		}

		if (m_fill)
		{
			Rect newRect = { 0, 0, maxWidth + (2 * GetShrinkFactor()), (int)m_nodes.size() * TTF_FontLineSkip(m_font) + (2 * GetShrinkFactor()) };
			if (!newRect.IsEqual(&m_rect))
			{
				m_rect = newRect;
				GetParentWnd()->GetScrollBars()->RefreshScrollBarStatus();
			}
		}
	}

	bool Tree::HandleEvent(SDL_Event * e)
	{
		return false;
	}

	HitResult Tree::HitTest(const PointRef pt)
	{
		return HIT_NOTHING;
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

	struct Tree::shared_enabler : public Tree
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Tree(std::forward<Args>(args)...)
		{
		}
	};
}