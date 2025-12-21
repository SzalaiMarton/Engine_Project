#pragma once

#include <filesystem>

struct CustomTexture {
	std::string name;
	sf::Texture* texture;

	CustomTexture(std::string_view name, sf::Texture* texture, bool isFileName);
};

namespace Textures {
	extern std::filesystem::path assetRootPath;
	extern std::vector<CustomTexture*> loadedTextures;
	sf::Texture* getTexture(std::string_view textureName);
	void loadTextures(std::filesystem::path innerRoot = assetRootPath);
	void printLoadedTextures();
}