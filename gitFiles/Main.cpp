#include <SFML/Graphics.hpp>

#include<iostream>
#include<cmath>
#include"playField.h"
#include"hitObject.h"
//#include"beatMap.h"

int main()
{
	sf::Vector2i screenSize = { 1920,1080 };
	sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "osu!",sf::Style::Fullscreen);
	
	sf::Clock runTime;
	//Objects created=================================================================================
	PlayField playField(screenSize);
	HitObject circle({ 65.0f,21.0f }, 4.2f, 1.8f, playField);
	HitObject slider({ 309.0f,311.0f }, 4.2f, 1.8f, playField, 'L', { 328.0f, 265.0f });
	//================================================================================================
	
	sf::RectangleShape rect;
	rect.setPosition(slider.getPos());
	rect.setSize({ 50 * 2.25,5 });
	rect.setRotation(std::atan2(265 - 311, 328 - 309) * 180 / 3.1415f);

	//Other utilities=================================================================================
	sf::Clock deltaClock;
	sf::Time deltaTime;
	//================================================================================================
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		//Render stuff to screen ====================================================
		window.clear();

		window.draw(playField.getPlayField());
		
		slider.approachTheCircle(deltaTime.asSeconds());
		slider.drawCircle(window);
		
		circle.approachTheCircle(deltaTime.asSeconds());
		circle.drawCircle(window);

		window.draw(rect);

		window.display();
		deltaTime = deltaClock.restart();
		//===============================================================================
	}
	return 0;
}