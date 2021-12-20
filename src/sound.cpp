
//  qnetwalk/sound.cpp
//  Copyright (C) 2013, Andi Peredri <andi@ukr.net>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "sound.h"

static int objcounter = 0;


Sound::Sound(const char * filename)
{
    if(!objcounter)
    {
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(22050, AUDIO_S16SYS, 1, 4096);
    }
    objcounter++;
    chunk = Mix_LoadWAV(filename);
}


void Sound::play()
{
    Mix_PlayChannel(-1, chunk, 0);
}


Sound::~Sound()
{
    Mix_FreeChunk(chunk);
    objcounter--;

    if(!objcounter)
    {
        Mix_CloseAudio();
	SDL_Quit();
    }
}
