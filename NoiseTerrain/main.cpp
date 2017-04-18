#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <noise/noise.h>

#include "config.h"
#include "tilemap.h"

using namespace noise;

// Returns indexes hand selected from the intput tilesheet
int getTileIndex(double noiseValue) {
    if (noiseValue <= WATER_THRESHOLD) return 28;
    if (noiseValue <= SANDY_THRESHOLD) return 19;
    if (noiseValue <= GRASS_THRESHOLD) return 220;
    if (noiseValue <= HILLS_THRESHOLD) return 73; // Best fit for "hilly" in this tileset is a forest
    if (noiseValue <= MOUNT_THRESHOLD) return 135;
    return 81; // SNOWY_THRESHOLD
}

int main() {
    // create the window
    sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Noisy Terrain");

    // Set up our perlin module
    module::Perlin perlinMod;
    srand(time(NULL));
    perlinMod.SetSeed(rand()); // Randomize the Perlin seed

    // Generate simple noise map
    double noiseMap[MAP_WIDTH*MAP_HEIGHT];
    for (unsigned int x = 0; x < MAP_WIDTH; x++) {
        for (unsigned int y = 0; y < MAP_HEIGHT; y++) {
            noiseMap[x+(y*MAP_WIDTH)] = perlinMod.GetValue((float)x/10, (float)y/10, 0.f);
        }
    }
    
    // Map it into an integer array
    int level[MAP_WIDTH*MAP_HEIGHT];
    for (unsigned int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++) {
        level[i] = getTileIndex(noiseMap[i]);
    }

    // create the tilemap from the level definition
    TileMap map;
    if (!map.load("tileset_2.gif", sf::Vector2u(TILE_SIZE, TILE_SIZE), level, MAP_WIDTH, MAP_HEIGHT))
        return EXIT_FAILURE;

    // run the main loop
    while (window.isOpen()) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // draw the map
        window.clear();
        window.draw(map);
        window.display();
    }

    return EXIT_SUCCESS;
}