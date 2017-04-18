#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <noise/noise.h>

#include "config.h"
#include "tilemap.h"

using namespace noise;

float z = 0.f;
sf::Vector2f offset(0.0, 0.0);

// Returns indexes hand selected from the intput tilesheet
int getTileIndex(double noiseValue) {
    if (noiseValue <= WATER_THRESHOLD) return 28;
    if (noiseValue <= SANDY_THRESHOLD) return 19;
    if (noiseValue <= GRASS_THRESHOLD) return 220;
    if (noiseValue <= HILLS_THRESHOLD) return 73; // Best fit for "hilly" in this tileset is a forest
    if (noiseValue <= MOUNT_THRESHOLD) return 135;
    return 81; // SNOWY_THRESHOLD
}

// Randomly reconfigures the terrain held in level
void generateTileMap(module::Perlin perlinMod, int* level) {
    // Generate simple noise map
    double noiseMap[MAP_WIDTH*MAP_HEIGHT];
    for (unsigned int x = 0; x < MAP_WIDTH; x++) {
        for (unsigned int y = 0; y < MAP_HEIGHT; y++) {
            noiseMap[x + (y*MAP_WIDTH)] = perlinMod.GetValue(((float)x / 10) + offset.x, ((float)y / 10) + offset.y, z);
        }
    }

    // Map it into an integer array
    for (unsigned int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++) {
        level[i] = getTileIndex(noiseMap[i]);
    }
}

int main() {
    // create the window
    sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Noisy Terrain");

    // Set up our perlin module
    module::Perlin perlinMod;
    srand(time(NULL));
    perlinMod.SetSeed(rand()); // Randomize the Perlin seed

    // Get the initial terrain
    int level[MAP_WIDTH*MAP_HEIGHT];
    generateTileMap(perlinMod, level);

    // create the tilemap from the level definition
    TileMap map;
    if (!map.load("tileset_2.gif", sf::Vector2u(TILE_SIZE, TILE_SIZE), level, MAP_WIDTH, MAP_HEIGHT))
        return EXIT_FAILURE;

    // run the main loop
    while (window.isOpen()) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::Space) { // Reseed the map
                    perlinMod.SetSeed(rand());
                } else if (event.key.code == sf::Keyboard::Left) {  // Travel backwards in time
                    z -= 0.05;
                } else if (event.key.code == sf::Keyboard::Right) { // Travel forwards in time
                    z += 0.05;
                } else if (event.key.code == sf::Keyboard::W) {     // Scroll the map
                    offset.y -= 0.1;
                } else if (event.key.code == sf::Keyboard::S) {     // Scroll the map
                    offset.y += 0.1;
                } else if (event.key.code == sf::Keyboard::A) {     // Scroll the map
                    offset.x -= 0.1;
                } else if (event.key.code == sf::Keyboard::D) {     // Scroll the map
                    offset.x += 0.1;
                }

                // Refresh the map
                generateTileMap(perlinMod, level);
                map.load("tileset_2.gif", sf::Vector2u(TILE_SIZE, TILE_SIZE), level, MAP_WIDTH, MAP_HEIGHT);
            }
        }

        // draw the map
        window.clear();
        window.draw(map);
        window.display();
    }

    return EXIT_SUCCESS;
}