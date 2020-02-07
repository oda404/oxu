#pragma once
#include <SFML/Graphics.hpp>
#include<vector>
#include<functional>

#include "hitObjectLoader.h"
#include "../oxuGameComponents/playField.h"
#include "../3rdParty/StandardCursor.hpp"
#include"soundHandler.h"

namespace oxu
{
    class GraphicsHandler
    {
    private:
        sf::Texture cursorTexture;
        sf::Sprite cursorSprite;
        uint16_t hitCircleIt = 0;
        unsigned int hitCircleCap = 0;

        HitObjectLoader *hitObjects;
        SoundHandler *mapSound;
        PlayField *playField;

        std::vector<std::vector<std::function<void (sf::RenderWindow &window, const float &dt)>>> sceneGraphicsHandlers;


    public:
        GraphicsHandler(HitObjectLoader *hitObjPtr, SoundHandler *soundPtr, PlayField *playFieldPtr):
        playField(playFieldPtr), mapSound(soundPtr), hitObjects(hitObjPtr)
        {
            std::vector<std::function<void(sf::RenderWindow &window, const float &dt)>> aux;
            //add main menu graphics handlers @ index 0===========================================================================
            aux.push_back([this](sf::RenderWindow &window, const float &dt) -> void { return this->drawMainMenu(window, dt); });

            sceneGraphicsHandlers.push_back(aux);

            aux.clear();
            //====================================================================================================================

            //add game graphics handlers @ index 1==================================================================================

            aux.push_back([this](sf::RenderWindow &window, const float &dt) -> void { return this->drawHitCircles(window, dt); });

            sceneGraphicsHandlers.push_back(aux);

            aux.clear();
            //======================================================================================================================

        }

        void loadHitObjects(HitObjectLoader hitObjectsObj)
        {
            hitObjects = new HitObjectLoader(hitObjectsObj);
            //delete heap allocation
        }

        void handleGraphics(sf::RenderWindow &window, const float &dt, const std::uint8_t & sceneID)
        {
            for(auto handler: sceneGraphicsHandlers[sceneID])
            {
                handler(window,dt);
            }
        }

        void drawHitCircles(sf::RenderWindow &window, const float &dt)
        {
			if (mapSound->getAudioPlayingOffset() >= hitObjects->hitCircleVector[hitCircleIt].getSpawnTime())
			{
				hitCircleIt++;
			}

            for(unsigned int i = hitCircleCap; i < hitCircleIt; i++)
            {
                if(!hitObjects->approachCircleVector[i].getApproachState())
                {
                    window.draw(hitObjects->approachCircleVector[i].getApproachCircle());
                    window.draw(hitObjects->hitCircleVector[i].getHitCircle());
                    hitObjects->approachCircleVector[i].approachTheCircle(dt, hitObjects->hitCircleVector[i].getHitCircleScale());
                }
                else
                    hitCircleCap++;
            }
        }

        void drawSliders(const float &dt,sf::RenderWindow &window)
        {
            for (unsigned int i = 0; i < hitObjects->sliderVector.size(); i++)
            {
                if (hitObjects->sliderVector[i].getSliderType() == 'L')
                {
                    if (mapSound->getAudioPlayingOffset() >= hitObjects->sliderVector[i].getSpawnTime() && !hitObjects->sliderApproachCircles[i].getApproachState())
                    {
                        window.draw(hitObjects->sliderVector[i].getHitCircle());
                        window.draw(hitObjects->sliderApproachCircles[i].getApproachCircle());
                        hitObjects->sliderApproachCircles[i].approachTheCircle(dt, hitObjects->sliderVector[i].getHitCircleScale());
                    }
                    else if (hitObjects->sliderApproachCircles[i].getApproachState())
                    {
                        window.draw(hitObjects->sliderVector[i].getHitCircle());
                        hitObjects->sliderVector[i].moveOnStraightSlider(dt, 0.300f, *playField, hitObjects->sliderApproachCircles[i]);
                    }
                }
                else if (hitObjects->sliderVector[i].getSliderType() == 'B')
                {
                    if (mapSound->getAudioPlayingOffset() >= hitObjects->sliderVector[i].getSpawnTime() && !hitObjects->sliderApproachCircles[i].getApproachState())
                    {
                        window.draw(hitObjects->sliderVector[i].getHitCircle());
                        window.draw(hitObjects->sliderApproachCircles[i].getApproachCircle());
                        hitObjects->sliderApproachCircles[i].approachTheCircle(dt, hitObjects->sliderVector[i].getHitCircleScale());
                    }
                    else if (hitObjects->sliderApproachCircles[i].getApproachState())
                    {
                        window.draw(hitObjects->sliderVector[i].getHitCircle());
                        hitObjects->sliderVector[i].moveOnBezierSlider(0.1f, *playField,dt ,hitObjects->sliderApproachCircles[i],window);
                    }
                }
            }          
        }

        void setCursor(sf::RenderWindow &window)
        {
        #ifdef _WIN32
            sf::Image im;
            im.loadFromFile("E:/visualproj/SFMLosuBootleg/skins/cursor.png");
            const sf::Uint8 *ptr = im.getPixelsPtr();
            sf::Cursor curs;
            curs.loadFromPixels(ptr, { 108,108 }, { 54,54 });
            window.setMouseCursor(curs);
        #else
            window.setMouseCursorVisible(false);
            cursorTexture.loadFromFile("/root/Documents/osuBootleg/skins/cursor.png");
            cursorSprite.setTexture(cursorTexture);
        #endif
        }

    #ifdef __linux__
        void drawCursor(sf::RenderWindow &window)
        {
            cursorSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
            window.draw(cursorSprite);
        }
    #endif

        void drawMainMenu(sf::RenderWindow &window, const float &dt)
        {
            static sf::Texture oLogo;
            static sf::Texture xLogo;
            static sf::Texture uLogo;
            oLogo.loadFromFile("/root/Documents/osuBootleg/textures/O.png");
            xLogo.loadFromFile("/root/Documents/osuBootleg/textures/X.png");
            uLogo.loadFromFile("/root/Documents/osuBootleg/textures/U.png");

            sf::Sprite o;
            sf::Sprite x;
            sf::Sprite u;

            o.setTexture(oLogo);
            x.setTexture(xLogo);
            u.setTexture(uLogo);

            o.setPosition(710,340);
            x.setPosition(860,340);
            u.setPosition(1010,340);

            window.draw(o);
            window.draw(x);
            window.draw(u);

            static sf::Font f;
            f.loadFromFile("/root/Documents/osuBootleg/textures/coolvetica.ttf");

            static sf::Text text("Click anywhere to continue!",f);
            text.setFillColor(sf::Color(0,0,0, 122));
            text.setCharacterSize(35);
            text.setPosition(725,750);

            window.draw(text);

        }

        void drawSongSelectMenu()
        {

        }

    };
}
