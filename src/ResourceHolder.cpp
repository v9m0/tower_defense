#include "ResourceHolder.hpp"


sf::Texture& ResourceHolder::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) return *it->second;
    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromFile(path)) {
        tex->create(2,2);
        sf::Image img;
        img.create(2,2,sf::Color::Magenta);
        tex->update(img);
    }
    tex->setSmooth(false);
    auto& ref = *tex;
    textures[path] = std::move(tex);
    return ref;
}


sf::Font& ResourceHolder::getFont(const std::string& path) {
    auto it = fonts.find(path);
    if (it != fonts.end()) return *it->second;
    auto f = std::make_unique<sf::Font>();
    f->loadFromFile(path);
    auto& ref = *f;
    fonts[path] = std::move(f);
    return ref;
}
