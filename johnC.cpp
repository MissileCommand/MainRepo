/* 
 * File:    johnC.cpp
 * Author:  John B. Crossley
 * Program: missileCommand (team Project)
 * Purpose: Functions for firing Dmissiles (defense missiles) up to 
 *          destroy/Stop Emissiles (Enemy Missiles)
 *          
 *          (April 2016)
 *          So far I just change the title bar text with mouse left and 
 *          right buttons to prove I am accessing my functions from 
 *          main file "missileCommand.cpp" and visa-versa
 * 
 *          (05/07/2016)
 *          Added "fireDefenseMissile" (used to be "movement" 
 *          inside "misileCommand.cpp")
 *          
 *          removed the extra empty lines+ from within the functions
 *          (5/5/16)
 * 
 *          5/13-14
 *          Added missile firing to mouse coords from 0,0 ONLY!
 *          Still need to make it fire from other locations
 * 
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

// extra comment

//#include "missileCommand.h"
#ifndef MISSILECOMMAND_H
#define MISSILECOMMAND_H
#include "missileCommand.h"
#endif



#ifndef _DANIELT_H_
#define _DANIELT_H_
#include "danielT.h"
#endif



using namespace std;

/*
 * 
 */
void changeTitle() 
{
    XStoreName(dpy, win, "335 Lab1 JBC Changed Title to prove a point");
}


// Previously called "movement"
// void renderDefenseMissile(Game *game, double xStart, double yStart)
void renderDefenseMissile(Game *game)
{
    float w, h;
    for (int i=0; i<game->n; i++) {
        Vec *c = &game->dMissile[i].s.center;
        w = 2;
        h = 2;
        glBegin(GL_QUADS);
            glVertex2i(c->x-w, c->y-h);
            glVertex2i(c->x-w, c->y+h);
            glVertex2i(c->x+w, c->y+h);
            glVertex2i(c->x+w, c->y-h);
        glEnd();
        glPopMatrix();
    }
    DefenseMissile *dMissilePtr;

    if (game->n <= 0)
        return;

    for (int i=0; i<game->n; i++) {
        dMissilePtr = &game->dMissile[i];

        // s.center refers to the Shape's "s" center's position
        // IE the position of the center of that particular Shape
        dMissilePtr->s.center.x += dMissilePtr->velocity.x;
        dMissilePtr->s.center.y += dMissilePtr->velocity.y;

    }
}

// 5/14 changes to make missile firing work
// seems OK now... :-)
void makeDefenseMissile(Game *game, int x, int y)
{

    if (game->n >= MAX_D_MISSILES)
        return;
	//std::cout << "makeDefenseMissile()" << x << " " << y << std::endl;
	
	DefenseMissile *dMissilePtr = &game->dMissile[game->n];

        // set speed of missile
        // 0.5 is a good start, 0.25 seemed a bit to slow & 5.0 
        // seemed insanely fast
        float missileSpeed = 0.75;
        
        // set start position of missile
        // Works now @ 2pm 05/14/16
        float xStart = 500.0;
        float yStart = 300.0;
        dMissilePtr->s.center.x = xStart;
        dMissilePtr->s.center.y = yStart;
        
        // set target of missile from mouse coords
        float xMissileTarget = x;
        float yMissileTarget = y;

        // do the math to find X,Y coords of mouse to pass to 
        // missile as target
        float dx = xMissileTarget - xStart;
        float dy = yMissileTarget - yStart;
        float dist = sqrt(dx*dx + dy*dy);
        dx /= dist;
        dy /= dist;
        float missileVelocityX = 0;
        float missileVelocityY = 0;

//        missileVelocityX = xStart + missileSpeed * dx;
        missileVelocityX = missileSpeed * dx;
        missileVelocityY = missileVelocityY + missileSpeed * dy;

        // Velocity is a vector quantity that refers to 
        // the rate at which an object changes its position.
        // The diff between X & Y determines the angle
        dMissilePtr->velocity.y = missileVelocityY;
        dMissilePtr->velocity.x = missileVelocityX;

        game->n++;
}

