// Copyright (c) Olaru Alexandru <olarualexandru404@gmail.com>
// Licensed under the MIT license found in the LICENSE file in the root of this repository.

#pragma once

#include<SDL2/SDL_render.h>
#include<SDL2/SDL_ttf.h>

#include<atomic>
#include<mutex>

#include<oXu/components/text.hpp>

#include<oXu/beatmap/songManager.hpp>
#include<oXu/skin/skinManager.hpp>

#include<oXu/utils/logger.hpp>

#include<oXu/core/status.hpp>
#include<oXu/core/threading/threads.hpp>
#include<oXu/core/dirs.hpp>

namespace oxu
{
    class GraphicsHandler 
    {
    private:
        SDL_Renderer *renderer = NULL;
        SDL_GLContext context;
        SDL_Window *window = NULL;
        bool *windowState;

        std::atomic<bool> doneInit = false;

        SongManager *songManager;
        SkinManager skinManager;

        Skin *skin;
        Beatmap *beatmap;

        TTF_Font *font = NULL;

        TextBox graphicsThreadFPS;
        TextBox inputThreadFPS;

        Thread *thisThread;

    public:
        ~GraphicsHandler();

        void init(SDL_Window *window, bool *windowState, SongManager *songManager);

    private:
        bool initSDL();

        bool initThread();

        void startThread();

        void renderHitCircles();

        void renderThreadsInfo();
    };
}