#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>


class ResourceHolder {
public:
    sf::Texture& getTexture(const std::string& path);

    sf::Font& getFont(const std::string& path);
private:
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;
};
