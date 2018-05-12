#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "ResourceManager.h"
#include "Image.h"

namespace GUI
{
	SDL::RendererRef ResourceManager::m_renderer = nullptr;
	ResourceManager::FontList ResourceManager::m_fonts;
	ResourceManager::ImageList ResourceManager::m_images;

	ResourceManager & ResourceManager::Get()
	{
		static ResourceManager manager;
		return manager;
	}

	void ResourceManager::Init(SDL::RendererPtr & renderer)
	{
		m_renderer = renderer.get();
	}

	SDL::FontRef ResourceManager::LoadFont(const char * id, const char * fileName, size_t size)
	{		
		if (id == nullptr || fileName == nullptr)
		{
			throw std::invalid_argument("id or filename is null");
		}
		if (m_fonts.find(id) != m_fonts.end())
		{
			throw std::invalid_argument("font id already loaded: " + std::string(id));
		}

		SDL::FontPtr font = SDL::FontPtr(TTF_OpenFont(fileName, (int)size), SDL::sdl_deleter());
		SDL::FontRef ref = font.get();
		if (font != nullptr)
		{
			m_fonts[id] = std::move(font);
		}

		return ref;
	}

	SDL::FontRef ResourceManager::FindFont(const char * id)
	{
		auto it = m_fonts.find(id);
		if (it == m_fonts.end())
		{
			return nullptr;
		}

		return it->second.get();
	}

	GUI::ImageRef ResourceManager::LoadImage(const char * id, const char * fileName)
	{
		if (id == nullptr || fileName == nullptr)
		{
			throw std::invalid_argument("id or filename is null");
		}
		if (m_images.find(id) != m_images.end())
		{
			throw std::invalid_argument("image id already loaded: " + std::string(id));
		}

		GUI::ImagePtr image = GUI::Image::Create(m_renderer);
		if (image != nullptr && image->LoadFromFile(fileName))
		{			
			m_images[id] = image;
		}
		
		return image.get();
	}

	GUI::ImageRef ResourceManager::FindImage(const char * id)
	{
		auto it = m_images.find(id);
		if (it == m_images.end())
		{
			return nullptr;
		}

		return it->second.get();
	}

}