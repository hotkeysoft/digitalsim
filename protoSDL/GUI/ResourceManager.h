#pragma once
#include "Common.h"
#include "Color.h"
#include <string>
#include <map>

namespace GUI
{
	class ResourceManager
	{
	public:
		using FontList = std::map<std::string, SDL::FontPtr>;
		using ImageList = std::map<std::string, GUI::ImagePtr>;

		virtual ~ResourceManager() = default;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

		static ResourceManager & Get();

		void Init(SDL::RendererPtr & renderer);

		SDL::FontRef LoadFont(const char * id, const char * fileName, size_t size);
		SDL::FontRef FindFont(const char * id);

		GUI::ImageRef LoadImage(const char * id, const char * fileName);
		GUI::ImageRef FindImage(const char * id);

	protected:
		ResourceManager() : m_renderer(nullptr) {}
		SDL::RendererRef m_renderer;
		FontList m_fonts;
		ImageList m_images;
	};

	constexpr auto RES = &ResourceManager::Get;
}