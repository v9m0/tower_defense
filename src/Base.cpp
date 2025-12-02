#include "Base.hpp"

Base::Base() {
    shape.setSize({40,40});
    shape.setOrigin(20,20);
    shape.setFillColor(sf::Color(100,200,100));
}

void Base::update(float) {
    shape.setPosition(pos);
}

void Base::draw(sf::RenderTarget& win) {
    win.draw(shape);
}

sf::FloatRect Base::bounds() const {
    return shape.getGlobalBounds();
}

void Base::setSize(float size) {
    shape.setSize({size,size});
    shape.setOrigin(size/2.f,size/2.f);
}
