#include "SDL.h"
#include "Point.h"
#include "Rect.h"
#include "Image.h"
#include "Window.h"
#include "ResourceManager.h"
#include "Button.h"
#include <algorithm>

namespace GUI
{
	Button::Button(const char * id, RendererRef renderer, Rect rect, const char * label, ImageRef image, FontRef font) :
		Widget(id, renderer, nullptr, rect, label, image, font)
	{
	}

	ButtonPtr Button::Create(const char * id, RendererRef renderer, Rect rect, const char * label, ImageRef image, FontRef font)
	{
		auto ptr = std::make_shared<shared_enabler>(id, renderer, rect, label, image, font);
		return std::static_pointer_cast<Button>(ptr);
	}

	HitZone Button::HitTest(const PointRef)
	{
		return HitZone();
	}

	void Button::Draw()
	{
		if (m_parent == nullptr)
			return;

		Rect parent = m_parent->GetClientRect(false, true);

		DrawButton(&m_rect.Offset(&parent.Origin()), Color::C_WHITE, nullptr, true);
	}

	struct Button::shared_enabler : public Button
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Button(std::forward<Args>(args)...)
		{
		}
	};
}