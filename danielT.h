
#ifndef MISSILECOMMAND_H
#define MISSILECOMMAND_H
#include "missileCommand.h"
#endif



#ifndef _DANIELT_H_
#define _DANIELT_H_
using namespace std;


extern void eExplosionPhysics(Game *game);
extern void eMissilePhysics(Game *game, Structures *sh);
extern void nameInBox(float xpoint, float ypoint);
extern void createEMissiles(Game *game, float x, float y);
extern void renderEMissiles(Game *game);
extern void eMissileExplode(Game *game, int misnum);
extern void renderEExplosions(Game *game);
extern void createEExplosion(Game *game, float x, float y);

#endif 
