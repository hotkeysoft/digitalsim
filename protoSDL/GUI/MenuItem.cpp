#include <SDL.h>
#include "MenuItem.h"

namespace GUI
{
	MenuItem::MenuItem(RendererRef renderer) : Widget("MenuItem", renderer, nullptr, Rect(), nullptr)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
	}

	MenuItemPtr MenuItem::Create(RendererRef renderer)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer);
		return std::static_pointer_cast<MenuItem>(ptr);
	}

	void MenuItem::Draw(const RectRef rect)
	{
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