//Author: Jose Garcia
//Program: joseG.cpp
//Purpose: This program will render the background/foreground of our project
//

#ifndef MISSILECOMMAND_H
#define MISSILECOMMAND_H
#include "missileCommand.h"
#endif

#ifndef _JOSEG_H_
#define _JOSEG_H_
using namespace std;

extern GLuint cityTexture;

extern void initStruc(Game *game);
extern void renderStruc(Game *game);
extern void destroyCity(Game *game, int citynum);
extern void renderBackground(GLuint starsTexture);

#endif
