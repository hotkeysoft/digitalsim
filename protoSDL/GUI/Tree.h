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
		std::string GetLabel() const { return m_label; }
		void SetLabel(const char * label) { m_label = label ? label : ""; }

		ImagePtr GetImage() const { return m_image; }
		void SetImage(ImagePtr image) { m_image = image; }

		TreeNodeRef GetParent() const { return m_parent; }

	private:
		TreeNode(const char* label, ImagePtr image, TreeNodeRef parent) : m_image(image), m_label(label), m_parent(parent) {}

		ImagePtr m_image;
		std::string m_label;
		TreeNodeRef m_parent;

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

		Rect GetRect(bool relative = true, bool scrolled = true) const override;
		Rect GetClientRect(bool relative = true, bool scrolled = true) const override;

		bool HandleEvent(SDL_Event *) override;
		HitResult HitTest(const PointRef) override;
		void Draw() override;

		TreeNodeRef AddNode(const char * label, ImagePtr image = ImagePtr(), TreeNodeRef parent = nullptr);

	protected:
		Tree(const char* id, RendererRef renderer, FontRef font, bool fill);

		void DrawBackground(const GUI::RectRef &rect);
		TreeNodeList::const_iterator FindNode(TreeNodeRef) const;
		TreeNodeRef AddRootNode(const char * label, ImagePtr image = ImagePtr());

		bool m_fill;
		TreeNodeList m_nodes;

		struct shared_enabler;
	};
}