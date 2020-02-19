#pragma once
#include<SFML/Graphics.hpp>
#include<memory>

#include"../oxuGameHandlers/graphicsHandler.hpp"
#include"../oxuGameHandlers/soundHandler.h"
#include"../oxuGameHandlers/beatMapParser.h"
#include"../oxuGameHandlers/hitObjectManager.h"
#include"../oxuGameComponents/playField.h"
#include"../oxuGameHandlers/inputHandler.hpp"

namespace oxu
{
    class Game
    {
    private:
        sf::Vector2i screenSize = { 1920,1080 };
        std::shared_ptr<sf::RenderWindow> window;

        std::shared_ptr<PlayField> playField;
        std::shared_ptr<InputHandler> inputHandler;
        std::shared_ptr<GraphicsHandler> graphicsHandler;
        SoundHandler soundHandler;
        std::shared_ptr<HitObjectManager> hitObjects;

        std::uint8_t currentScene = 0;

        sf::Clock deltaClock;
	    sf::Time deltaTime;

    public:
        Game();

        void run();

    };
}