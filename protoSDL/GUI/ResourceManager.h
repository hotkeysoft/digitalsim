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
		static void Init(SDL::RendererPtr & renderer);

		static SDL::FontRef LoadFont(const char * id, const char * fileName, size_t size);
		static SDL::FontRef FindFont(const char * id);

		static GUI::ImageRef LoadImage(const char * id, const char * fileName);
		static GUI::ImageRef FindImage(const char * id);

	protected:
		ResourceManager() = default;
		static SDL::RendererRef m_renderer;
		static FontList m_fonts;
		static ImageList m_images;
	};
}