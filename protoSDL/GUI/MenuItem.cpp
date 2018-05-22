#include <SDL.h>
#include "MenuItem.h"
#include "Label.h"

namespace GUI
{
	MenuItem::MenuItem(RendererRef renderer, const char * id, const char * name) : Widget(id, renderer, nullptr, Rect(), name), m_opened(false)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
	}

	MenuItemPtr MenuItem::Create(RendererRef renderer, const char * id, const char * name)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer, id, name);
		return std::static_pointer_cast<MenuItem>(ptr);
	}

	void MenuItem::Init()
	{
		m_label = Label::CreateAutoSize("l", m_renderer, Rect(), m_text.c_str(), nullptr, Label::TEXT_AUTOSIZE_DEFAULT, LCF_MENUITEM);
		m_label->SetPadding(Dimension(8, 0));
		m_label->Init();
	}

	void MenuItem::Draw(const RectRef rect)
	{
	}

	bool MenuItem::Hit(PointRef pt)
	{
		return m_labelRect.PointInRect(pt);
	}

	struct MenuItem::shared_enabler : public MenuItem
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: MenuItem(std::forward<Args>(args)...)
		{
		}
	};
}