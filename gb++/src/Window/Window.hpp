#pragma once

#ifndef WINDOW
#define WINDOW

#include <cstdint>
#include <string>

#include <SFML/Graphics.hpp>

class Window {
    unsigned int screenWidth;
    unsigned int screenHeight;
    unsigned int scalingFactor = 4;
    const uint8_t* fbPointer;
    std::string windowTitle;

    sf::RenderWindow* emuWindow;
    sf::Texture fbTexture;
    sf::Sprite fbSprite;
public:
    Window(unsigned int _width, unsigned int _height, uint8_t* _fbPointer, std::string _title = "gb++");
    ~Window();

    bool windowEvent(sf::Event& emuEvent);

    void draw();
};

#endif

