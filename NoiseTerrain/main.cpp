#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <noise/noise.h>

#include "config.h"
#include "tilemap.h"

using namespace noise;

sf::Vector3<double> position(0.0, 0.0, 0.0);
double speed = 0.1;
int seed = 0;

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
            noiseMap[x + (y*MAP_WIDTH)] = perlinMod.GetValue(((double)x / 10) + position.x, ((double)y / 10) + position.y, position.z);
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
    seed = rand();
    perlinMod.SetSeed(seed); // Randomize the Perlin seed

    // Get the initial terrain
    int level[MAP_WIDTH*MAP_HEIGHT];
    generateTileMap(perlinMod, level);

    // create the tilemap from the level definition
    TileMap map;
    if (!map.load("tileset_2.gif", sf::Vector2u(TILE_SIZE, TILE_SIZE), level, MAP_WIDTH, MAP_HEIGHT))
        return EXIT_FAILURE;

    // Initialize text elements
    sf::Font font;
    if (!font.loadFromFile("sansation.ttf")) {
        return EXIT_FAILURE;
    }

    sf::Text scrollText("Use WASD to scroll", font, 25);
    scrollText.setColor(sf::Color::White);
    scrollText.setPosition(512, 0);

    sf::Text changeTimeText("Use [left] or [right] to modify time", font, 25);
    changeTimeText.setColor(sf::Color::White);
    changeTimeText.setPosition(512, 25);

    sf::Text changeSeedText("Use [space] to randomize the seed", font, 25);
    changeSeedText.setColor(sf::Color::White);
    changeSeedText.setPosition(512, 50);

    sf::Text positionText("Position: (0, 0)", font, 25);
    positionText.setColor(sf::Color::White);
    positionText.setPosition(512, 100);

    sf::Text timeText("Time: 0.000", font, 25);
    timeText.setColor(sf::Color::White);
    timeText.setPosition(512, 125);

    sf::Text seedText("Seed: 0.000", font, 25);
    seedText.setColor(sf::Color::White);
    seedText.setPosition(512, 150);

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
                    seed = rand();
                    perlinMod.SetSeed(seed);
                } else if (event.key.code == sf::Keyboard::Left) {  // Travel backwards in time
                    position.z -= 0.05;
                } else if (event.key.code == sf::Keyboard::Right) { // Travel forwards in time
                    position.z += 0.05;
                } else {
                    // Scroll the map
                    if (event.key.code == sf::Keyboard::W) {
                        position.y -= speed;
                    }
                    if (event.key.code == sf::Keyboard::S) {
                        position.y += speed;
                    }
                    if (event.key.code == sf::Keyboard::A) {
                        position.x -= speed;
                    }
                    if (event.key.code == sf::Keyboard::D) {
                        position.x += speed;
                    }
                }

                // Refresh the map
                generateTileMap(perlinMod, level);
                map.load("tileset_2.gif", sf::Vector2u(TILE_SIZE, TILE_SIZE), level, MAP_WIDTH, MAP_HEIGHT);
            }
        }
        // Update Text
        positionText.setString("Position: (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
        timeText.setString("Time: " + std::to_string(position.z));
        seedText.setString("Seed: " + std::to_string(seed));

        // draw the map
        window.clear();
        window.draw(map);
        window.draw(scrollText);
        window.draw(changeTimeText);
        window.draw(changeSeedText);
        window.draw(positionText);
        window.draw(timeText);
        window.draw(seedText);
        window.display();
    }

    return EXIT_SUCCESS;
}