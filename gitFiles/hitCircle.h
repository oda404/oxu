#pragma once
#include"Texture.h"

class HitCircleSprites : Textures
{
public:
	HitCircleSprites(const sf::Vector2f &newOrigin,const sf::Vector2f &position, const float &CS,float const &osuPx)
	{
		this->hitCircle.setScale(((109-9*CS)*osuPx)/128, ((109 - 9 * CS)*osuPx) / 128);
		
		this->hitCircle.setTexture(hitCircleTexture);
		//Set the origin to center of the circle, move the circle there======
		this->hitCircle.setOrigin(newOrigin);
		this->hitCircle.move(newOrigin - this->hitCircle.getOrigin());
		//===================================================================
		this->hitCircle.setPosition(position);
	}
	
	sf::Vector2f getHitCircleScale() const
	{
		return this->hitCircle.getScale();
	}
	
	sf::Sprite getHitCircle() const
	{
		return this->hitCircle;
	}
	
private:
	sf::Sprite hitCircle;
};