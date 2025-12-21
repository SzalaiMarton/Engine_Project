#include "stdafx.h"

namespace Textures {
	std::vector<CustomTexture*> loadedTextures = {};
	std::filesystem::path assetRootPath = "assets";
}

CustomTexture::CustomTexture(std::string_view name, sf::Texture* texture, bool isFileName) : texture(texture) {
	if (isFileName) {
		this->name = name.substr(0, name.find('.'));
	}
	else {
		this->name = name;
	}
}

void Textures::printLoadedTextures() {
	for (auto& e : loadedTextures) {
		LOG(e->name);
	}
}

sf::Texture* Textures::getTexture(std::string_view textureName) {
	for (auto& texture : loadedTextures) {
		if (texture->name == textureName) {
			return texture->texture;
		}
	}
	throw TextureError("Texture doesn't exist.");
}

void Textures::loadTextures(std::filesystem::path innerRoot) {
	if (std::filesystem::exists(innerRoot)) {
		for (auto& element : std::filesystem::directory_iterator(innerRoot)) {
			if (std::filesystem::is_directory(element)) {
				Textures::loadTextures(element);
			}
			else {
				try {
					sf::Texture* t = new sf::Texture(element);
					Textures::loadedTextures.emplace_back(new CustomTexture(element.path().filename().string(), t, true));
				}
				catch (sf::Exception e) {
					ERROR(e.what());
				}
			}
		}
	}
}
