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
	Tree::Tree(const char * id, RendererRef renderer, FontRef font, bool fill) :
		Widget(id, renderer, nullptr, Rect(), nullptr, nullptr, font), m_fill(fill)
	{
		m_backgroundColor = Color::C_WHITE;
		m_margin = 5;
	}

	void Tree::Init()
	{
		//RenderLabel();
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

		for (auto node : m_nodes)
		{
			std::cout << "node:" << node->GetLabel() << std::endl;
		}
	}

	void Tree::DrawBackground(const RectRef &rect)
	{
		Rect fillRect = m_rect;
		fillRect.h = std::max(rect->h, m_rect.h);
		fillRect.w = std::max(rect->w, m_rect.w);

		DrawFilledRect(rect, m_backgroundColor);
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

		TreeNodeRef root = new TreeNode(label, image, nullptr);
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

		TreeNodeRef root = new TreeNode(label, image, parent);
		m_nodes.insert(++iter, root);
		return root;
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