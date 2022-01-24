#include "Window.hpp"

Window::Window(unsigned int _width, unsigned int _height, uint8_t* _fbPointer, std::string _title) : fbPointer(_fbPointer) {
    screenWidth = _width;
    screenHeight = _height;
    windowTitle = _title;

    emuWindow = new sf::RenderWindow(sf::VideoMode(screenWidth * scalingFactor, screenHeight * scalingFactor), windowTitle, sf::Style::Titlebar | sf::Style::Close);

    auto screen = sf::VideoMode::getDesktopMode();
    sf::Vector2i windowPos((screen.width / 2) - (emuWindow->getSize().x / 2), (screen.height / 2) - (emuWindow->getSize().y / 2));
    emuWindow->setPosition(windowPos);

    fbTexture.create(screenWidth, screenHeight);
    fbSprite = sf::Sprite(fbTexture);
    fbSprite.setScale(sf::Vector2f(scalingFactor, scalingFactor));
}

Window::~Window() {
    emuWindow->close();
    delete emuWindow;
}

bool Window::windowEvent(sf::Event& emuEvent) {
    return emuWindow->pollEvent(emuEvent);
}

void Window::draw() {
    fbTexture.update(fbPointer);
    emuWindow->draw(fbSprite);
    emuWindow->display();
}