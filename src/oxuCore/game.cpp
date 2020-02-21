#include"game.hpp"

oxu::Game::Game()
{
    window = std::make_shared<sf::RenderWindow>(sf::VideoMode(screenSize.x, screenSize.y), "oXu");
    window->setFramerateLimit(120);

	playField =  std::make_shared<PlayField>(window->getSize());

    hitObjects = std::make_shared<HitObjectManager>(playField.get());

    inputHandler = std::make_shared<InputHandler>(hitObjects.get(), &soundHandler, &mapManager);

    graphicsHandler = std::make_shared<GraphicsHandler>(inputHandler.get(), hitObjects.get(), &soundHandler, playField.get(), &mapManager);
    graphicsHandler->setCursor(window.get());
}

void oxu::Game::run()
{
    while (window->isOpen())
	{
		deltaTime = deltaClock.restart();

		sf::Event event;
		while (window->pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed:
				soundHandler.setVolumeToDefault();
				soundHandler.freeAudio();
				window->close();
				break;

			default:
				break;
			}
		}

		if(currentScene == 2)
			window->clear();
		else
			window->clear(sf::Color(100,100,100,255));
				
	#ifdef __linux__
		graphicsHandler->drawCursor(*window.get());
	#endif
		
		//================  Actual scene handling  ==================
		soundHandler.handleSound(currentScene);

		graphicsHandler->handleGraphics(*window.get(), deltaTime.asSeconds(), currentScene);

		inputHandler->handleInput(*window.get(), currentScene);
		//===========================================================

		window->display();
	}
}