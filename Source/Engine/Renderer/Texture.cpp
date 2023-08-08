#include "Texture.h"
#include "Renderer.h"
#include "SDL2-2.28.0/include/SDL_image.h"
#include "Core/Logger.h"


namespace kiko
{
	Texture::~Texture()
	{
		if (m_texture) SDL_DestroyTexture(m_texture);
	}

	bool Texture::Load(const std::string filename, class Renderer& renderer)
	{
		SDL_Surface* surface = IMG_Load(filename.c_str());
		if (!surface)
		{
			// LOG_WARNING
			WARNING_LOG("NULL surface.");
			return false;
		}
		m_texture = SDL_CreateTextureFromSurface(renderer.m_renderer, surface);
		SDL_FreeSurface(surface);
		if (!m_texture)
		{
			// LOG_WARNING
			WARNING_LOG("NULL texture.");
			return false;
		}
		return true;
	}

	bool Texture::Create(std::string filename, ...)
	{
		va_list args;
		
		va_start(args, filename);

		Renderer& renderer = va_arg(args, Renderer);
		
		va_end(args);


		return Load(filename, renderer);
	}

	vec2 Texture::GetSize()
	{
		// ASSERT texture is not null
		ASSERT_LOG(this->m_texture != NULL, "Texture can NOT be null.");
		SDL_Point point;
		// https://wiki.libsdl.org/SDL2/SDL_QueryTexture
		SDL_QueryTexture(this->m_texture, NULL, NULL, &point.x, &point.y);
		return vec2{ point.x, point.y };
	}

	
}