#include <SDL.h>
#include <SDL_image.h>
#include "Image.h"

namespace GUI
{
	Image::Image(RendererRef & renderer) : m_renderer(renderer)
	{
		if (m_renderer == nullptr)
		{
			throw std::invalid_argument("no renderer");
		}
	}

	ImagePtr Image::Create(RendererRef & renderer)
	{
		auto ptr = std::make_shared<shared_enabler>(renderer);
		return std::static_pointer_cast<Image>(ptr);
	}

	bool Image::LoadFromFile(const char* fileName)
	{
		m_texture = TexturePtr(IMG_LoadTexture(m_renderer, fileName), sdl_deleter());
		if (m_texture)
		{
			SDL_QueryTexture(m_texture.get(), NULL, NULL, &m_width, &m_height);
			return true;
		}

		m_width = 0;
		m_height = 0;
		return false;
	}

	void Image::Draw(SDL_Point pos)
	{
		SDL_Rect rect = { pos.x, pos.y, m_width, m_height };
		if (m_texture && m_renderer)
		{
			SDL_RenderCopy(m_renderer, m_texture.get(), NULL, &rect);
		}
	}
	
	void Image::Draw(SDL_Rect pos)
	{
		SDL_Rect src = { 0, 0, pos.w, pos.h };
		if (m_texture && m_renderer)
		{
			SDL_RenderCopy(m_renderer, m_texture.get(), &src, &pos);
		}
	}

	struct Image::shared_enabler : public Image
	{
		template <typename... Args>
		shared_enabler(Args &&... args)
			: Image(std::forward<Args>(args)...)
		{
		}
	};
}