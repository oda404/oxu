// Copyright (c) Olaru Alexandru <olarualexandru404@gmail.com>
// Licensed under the MIT license found in the LICENSE file in the root of this repository.

#include "game.hpp"

bool oxu::Game::w_isClosed = false;

oxu::Game &oxu::Game::getInstance()
{
	static Game instance;
	return instance;
}

int oxu::Game::init()
{
	/* Initiate the logger */
	Logger::init();

	/* Initialize SDL */
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
	{
		LOG_ERR(SDL_GetError());
		StatusCodes::statusCode = StatusCodes::SDL_INIT_FAIL;
		return false;
	}

	if( IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0 )
	{
		LOG_ERR(IMG_GetError());
		StatusCodes::statusCode = StatusCodes::IMG_INIT_FAIL;
		return false;
	}

	Scaling::screenSize = { 1920, 1080 };
	Scaling::oxuPx = Scaling::screenSize.y / 480.f;

	/* Create the window */
	window = SDL_CreateWindow(
	"oXu!",                      // window name
	SDL_WINDOWPOS_CENTERED,      // window pos X
	SDL_WINDOWPOS_CENTERED,      // window pos Y
	Scaling::screenSize.x,   // screen width
	Scaling::screenSize.y,   // screen height
	0                            // flags
	);

	if(!window)
	{
		LOG_ERR(SDL_GetError());
		StatusCodes::statusCode = StatusCodes::WINDOW_CREATE_FAIL;
		return false;
	}
	
	/* Enumerate all skins */
	skinsManager.enumSkins();

	/* Load the skins images onto surfaces */
	Textures::getInstance().loadSkinSurfaces(skinsManager.getSkinPath(0));

	/* Set the cursor png */
	cursor.set(skinsManager.getSkinPath(0));

	/* Enumerate all the beatmaps */
	beatmapManager.enumBeatMaps();
	/* Load info/objects from song 0 map 0 */
	beatmapManager.loadMapInfo(0, 0);
	beatmapManager.loadHitObjects(0, 0);

	/* Initiate the graphics handler */
	/* This spawns another thread */
	graphicsHandler.init(window, &graphicsThread, &w_isClosed, &beatmapManager);

	/* Initiate the sound handler */
	if(!soundHandler.init(&beatmapManager))
	{
		return false;
	}

	soundHandler.loadMusic((beatmapManager.getSongPath(0) + '/' + beatmapManager.getBeatmapInfo().getGeneralAttr("AudioFilename")).c_str());
	soundHandler.setMusicVolume(5);
	
	soundHandler.loadSoundEffects(skinsManager.getSkinPath(0));
	soundHandler.setEffectsVolume(5);

	soundHandler.playMusic();

	/* Initiate the input handler */
	/* The input handler uses the main thread and does not spawn another one */
	inputHandler.init(&beatmapManager);
	
	return true;
}

void oxu::Game::loop()
{
	HitObjectsInfo &objInfo = beatmapManager.getObjectsInfo();
	
	while(!w_isClosed)
	{
		calculateDeltaTime();

		/* event/input handling */
		inputHandler.handleInput(w_isClosed);

		objInfo.checkHitCircleBounds();

		limitFPS();
	}
	
	graphicsThread->join();
};

void oxu::Game::clean()
{
	if(StatusCodes::statusCode != StatusCodes::OK)
	{
		LOG_WARN("Exiting with return status {0}", StatusCodes::statusCode);
	}
	else
	{
		LOG_INFO("Exiting gracefully. Hai noroc!");
	}

	/* destroy the window */
	SDL_DestroyWindow(window);
	window = NULL;

	IMG_Quit();

	/* quit all SDL subsystems */
	SDL_Quit();
}

void oxu::Game::calculateDeltaTime()
{
	startTick = SDL_GetTicks();
	deltaTime = (double)(startTick - lastTick) / 1000.0f;
	lastTick  = startTick;
}

void oxu::Game::limitFPS()
{
	if(1000 / maxFPS > SDL_GetTicks() - startTick)
	{
		SDL_Delay(1000 / maxFPS - SDL_GetTicks() + startTick);
	}
}