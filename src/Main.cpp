#include <SFML/Graphics.hpp>

#include"oxuGameComponents/playField.h"
#include"oxuGameHandlers/beatMapParser.h"
#include"oxuGameHandlers/hitObjectLoader.h"
#include"oxuGameHandlers/graphicsHandler.hpp"
#include"oxuGameHandlers/soundHandler.h"
#include"oxuCore/sceneManager.h"

int main()
{
	sf::Vector2i screenSize = { 1920,1080 };
	sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "oXu");
	window.setFramerateLimit(120);

	oxu::PlayField playField(screenSize);

	oxu::BeatMapParser map;

	oxu::HitObjectLoader aw;
	aw.createHitObjects(map, playField);

	oxu::SoundHandler soundHandler;
	soundHandler.loadAudioFile("yomi.mp3");
	soundHandler.setAudioVolume(0.3f);
	soundHandler.playAudio();

	oxu::GraphicsHandler graph(&aw, &soundHandler, &playField);
	graph.setCursor(window);

	//rect.setRotation(std::atan2(265.0f - 311.0f, 328.0f - 309.0f) * 180.0f / 3.1415f);

	sf::Clock deltaClock;
	sf::Time deltaTime;

	oxu::SceneManager sc(&graph);


	while (window.isOpen())
	{
		deltaTime = deltaClock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
	#ifdef __linux__
		graph.drawCursor(window);
	#endif

		sc.handleCurrentScene(window,deltaTime.asSeconds());

		window.display();

	}

	soundHandler.freeAudio();

	return 0;
}