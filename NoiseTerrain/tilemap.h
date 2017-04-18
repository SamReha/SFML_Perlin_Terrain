// Adapted from a tutorial at https://www.sfml-dev.org/tutorials/2.4/graphics-vertex-array.php

#pragma once
#include <SFML/Graphics.hpp>

class TileMap : public sf::Drawable, public sf::Transformable {
private:
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height);
};