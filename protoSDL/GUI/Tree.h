#pragma once
#include "Common.h"
#include "Widget.h"
#include "Rect.h"
#include "WindowManager.h"
#include <string>
#include <list>

namespace GUI
{
	using TreeNodeList = std::list<TreeNodeRef>;

	class TreeNode
	{
	public:
		std::string GetText() const { return m_text; }
		void SetText(const char * text) { m_text = text ? text : ""; Render(); }

		TreeNodeRef GetParent() const { return m_parent; }

		bool IsOpen() const { return m_opened; } // True if node is open to show children		
		bool IsSelected() const { return m_selected; } // True if node is selected
		bool IsVisible() const; // False is any of parent nodes is closed

		bool Hit(PointRef pt) { return m_labelRect.PointInRect(pt); }

	private:
		bool m_opened;
		bool m_selected;

		TreeNode(RendererRef renderer, const char* label, ImageRef opened, ImageRef closed, TreeNodeRef parent, TreeRef tree);
		void Render();

		ImageRef m_openedImage;
		ImageRef m_closedImage;
		
		TreeNodeRef m_parent;

		std::string m_text;
		LabelPtr m_label;
		Rect m_labelRect;

		TreeRef m_tree;
		int m_depth;

		RendererRef m_renderer;

		friend class Tree;
	};

	class Tree : public Widget
	{
	public:
		virtual ~Tree() = default;
		Tree(const Tree&) = delete;
		Tree& operator=(const Tree&) = delete;
		Tree(Tree&&) = delete;
		Tree& operator=(Tree&&) = delete;

		void Init() override;

		void SetIndent(int indent) { m_indent = clip(indent, 0, 255); }
		int GetIndent() const { return m_indent; }

		// Creates a tree that fills the whole parent window
		static TreePtr Create(const char* id, RendererRef renderer, int lineHeight = 20, FontRef font = nullptr);

		WindowRef GetParentWnd() { return dynamic_cast<WindowRef>(m_parent); }

		Rect GetRect(bool relative = true, bool scrolled = true) const override;
		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;
		void Draw() override;

		TreeNodeRef AddNode(const char * label, TreeNodeRef parent = nullptr);
		TreeNodeRef AddNode(const char * label, ImageRef image, TreeNodeRef parent = nullptr);
		TreeNodeRef AddNode(const char * label, ImageRef opened, ImageRef closed, TreeNodeRef parent = nullptr);
		
		void OpenNode(TreeNodeRef node, bool open = true);

		bool NodeHasChildren(TreeNodeRef node);
		bool NodeHasNextSibling(TreeNodeRef node);
		bool NodeHasPreviousSibling(TreeNodeRef node);
	
		void SelectNode(TreeNodeRef node);
		TreeNodeRef GetSelectedNode() const;

		void MoveSelectionRel(int16_t deltaY);
		void MoveSelectionPage(int16_t deltaY);
		void OpenSelection();
		void CloseSelection();

	protected:
		Tree(const char* id, RendererRef renderer, bool fill, int lineHeight, FontRef font);

		void RenderNodes();
		int GetVisibleLineCount();

		void DrawBackground(const GUI::RectRef &rect);
		void DrawTree(const GUI::RectRef &rect);
		void DrawNode(const GUI::RectRef &rect, int line, TreeNodeRef node);
		TreeNodeList::const_iterator FindNode(TreeNodeRef) const;
		TreeNodeList::const_iterator FindSelectedNode() const;
		TreeNodeRef AddRootNode(const char * label, ImageRef opened, ImageRef closed);
		TreeNodeRef NodeAt(PointRef pt);

		void ScrollSelectionIntoView();

		bool m_fill;
		TreeNodeList m_nodes;

		int m_lineHeight;
		int m_indent;

		struct shared_enabler;
	};
}