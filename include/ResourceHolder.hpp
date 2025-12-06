#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>


// кэш текстур и шрифтов
class ResourceHolder {
public:
    // вернуть текстуру из кэша или загрузить
    sf::Texture& getTexture(const std::string& path);

    // вернуть шрифт из кэша или загрузить
    sf::Font& getFont(const std::string& path);
private:
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;
};
