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
		std::string GetLabel() const { return m_label; }
		void SetLabel(const char * label) { m_label = label ? label : ""; Render(); }

		ImagePtr GetImage() const { return m_image; }
		void SetImage(ImagePtr image) { m_image = image; }

		TreeNodeRef GetParent() const { return m_parent; }

		bool IsOpen() const { return m_open; } // True is node is open to show children		
		bool IsVisible() const; // False is any of parent nodes is closed

	private:
		bool m_open;

		TreeNode(const char* label, ImagePtr image, TreeNodeRef parent, TreeRef tree);
		void Render();

		ImagePtr m_image;
		std::string m_label;
		TreeNodeRef m_parent;

		TexturePtr m_texture;
		Rect m_rect; // Node dimensions, from rendering

		Rect m_drawRect; // Node position on screen

		TreeRef m_tree;
		int m_depth;

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

		// Creates a tree that fills the whole parent window
		static TreePtr Create(const char* id, RendererRef renderer, FontRef font = nullptr);

		WindowRef GetParentWnd() { return dynamic_cast<WindowRef>(m_parent); }

		Rect GetRect(bool relative = true, bool scrolled = true) const override;
		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;
		void Draw() override;

		TreeNodeRef AddNode(const char * label, ImagePtr image = ImagePtr(), TreeNodeRef parent = nullptr);

		void OpenNode(TreeNodeRef node, bool open = true);

		bool NodeHasChildren(TreeNodeRef node);
		bool NodeHasNextSibling(TreeNodeRef node);
		bool NodeHasPreviousSibling(TreeNodeRef node);
	
	protected:
		Tree(const char* id, RendererRef renderer, FontRef font, bool fill);

		void RenderNodes();
		int GetVisibleLineCount();

		void DrawBackground(const GUI::RectRef &rect);
		void DrawTree(const GUI::RectRef &rect);
		void DrawNode(const GUI::RectRef &rect, int line, TreeNodeRef node);
		TreeNodeList::const_iterator FindNode(TreeNodeRef) const;
		TreeNodeRef AddRootNode(const char * label, ImagePtr image = ImagePtr());

		bool m_fill;
		TreeNodeList m_nodes;

		int m_lineHeight;
		int m_charWidth;

		struct shared_enabler;
	};
}