
//  qnetwalk/sound.h
//  Copyright (C) 2013, Andi Peredri <andi@ukr.net>

#ifndef SOUND_H
#define SOUND_H

struct Mix_Chunk;

class Sound
{
public:
    Sound(const char * filename);
    ~Sound();
    void play();

private:
    Mix_Chunk *chunk;
};

#endif
