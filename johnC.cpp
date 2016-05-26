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
 *          inside "missileCommand.cpp")
 *          
 *          removed the extra empty lines+ from within the functions
 *          (5/5/16)
 * 
 *          5/13-14
 *          Added missile firing to mouse coords from 0,0 ONLY!
 *          Still need to make it fire from other locations
 * 
 *          5/14-15/16
 *          added code to make defense missiles lime green and bigger
 *          also made them stop at mouse click location
 * 
 *          5/25/16
 *          Changed explosion coords to mouse location instead of missile 
 *          location to be more accurate. (SEE TODO #1)
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
extern void dMissileRemove(Game *game, int dMissilenumber);
extern void createEExplosion(Game *game, float x, float y);

//void changeTitle() 
//{
//    XStoreName(dpy, win, "335 Lab1 JBC Changed Title to prove a point");
//}

void renderDefenseMissile(Game *game)
{
    DefenseMissile *dMissilePtr;
    float w, h;
    
    for (int i=0; i<game->numberDefenseMissiles; i++) {
        Vec *c = &game->dMissile[i].shape.center;
        w = 5;
        h = 5;
        glColor3f(game->dMissile[i].color[0],
                game->dMissile[i].color[1], 
                game->dMissile[i].color[2]);

        glBegin(GL_QUADS);
            glVertex2i(c->x-w, c->y-h);
            glVertex2i(c->x-w, c->y+h);
            glVertex2i(c->x+w, c->y+h);
            glVertex2i(c->x+w, c->y-h);

            glEnd();
        glPopMatrix();
    }
    

    if (game->numberDefenseMissiles <= 0)
        return;

    for (int i=0; i<game->numberDefenseMissiles; i++) {
        dMissilePtr = &game->dMissile[i];
        
        // TODO #1: missile goes too far past mouse coords before exploding
        // temp workaround is to set explosion coords to mouse coords 
        // directly instead of using missile coords to place explosion
        if (dMissilePtr->destinationY >= dMissilePtr->shape.center.y ) {
//        if (dMissilePtr->destinationY >= dMissilePtr->shape.center.y || 
//                dMissilePtr->destinationX >= dMissilePtr->shape.center.x) {

        // shape.center refers to the Shape center position
        // IE the position of the center of that particular Shape
        dMissilePtr->shape.center.x += dMissilePtr->velocity.x;
        dMissilePtr->shape.center.y += dMissilePtr->velocity.y;
            
        } else {
            
//            // test location of explosion vs mouse pick coords
//            cout << "X,Y Explosion coords:" << 
//                    dMissilePtr->shape.center.x << 
//                "," << dMissilePtr->shape.center.y << endl; 
            
            dMissileRemove(game, i);
            
        }

    }
}


void dMissileRemove(Game *game, int dMissilenumber)
{
    DefenseMissile *dMissilePtr = &game->dMissile[dMissilenumber];
    
   
    // cant seem to make them a different color???
    createEExplosion(game,  dMissilePtr->destinationX,
                            dMissilePtr->destinationY);

    // changed to the lines above instead of these as temp work around
    // see also TODO #1
//    // cant seem to make them a different color???
//    createEExplosion(game,  dMissilePtr->shape.center.x,
//                            dMissilePtr->shape.center.y);

    
    //delete defense missile
    game->dMissile[dMissilenumber] = 
        game->dMissile[game->numberDefenseMissiles - 1];


    game->numberDefenseMissiles--;
}



// 5/14 changes to make missile firing work
// seems OK now... :-)
void makeDefenseMissile(Game *game, int x, int y)
{

    if (game->numberDefenseMissiles >= MAX_D_MISSILES)
        return;
    //std::cout << "makeDefenseMissile()" << x << " " << y << std::endl;
    
    DefenseMissile *dMissilePtr = 
                &game->dMissile[game->numberDefenseMissiles];
        dMissilePtr->shape.width = 10;
        dMissilePtr->shape.height = 10;
        dMissilePtr->shape.radius = 10;

        dMissilePtr->color[0] = 0;
        dMissilePtr->color[1] = 255;
        dMissilePtr->color[2] = 0;

        // set target of missile from mouse coords
        dMissilePtr->destinationX = x;
        dMissilePtr->destinationY = y;
        
//        // test location of explosion vs mouse pick coords
//        cout << "X,Y Mouse coords:" << dMissilePtr->destinationX << 
//                "," << dMissilePtr->destinationY << endl;
//        
        
        
        // set speed of missile
        // 0.5 is a good start, 0.25 seemed a bit to slow & 5.0 
        // seemed insanely fast (BEFORE setting my NVidia card to 
        // "Sync to VBlank")
        
        // Moved to main game struct
        // float defMissileSpeed = 40;
        // game->defMissileSpeed = 40;
        
        
        
        // set start position of missile
        // Works now @ 2pm 05/14/16
        float xStart = 500.0;
        float yStart = 50.0;
        dMissilePtr->shape.center.x = xStart;
        dMissilePtr->shape.center.y = yStart;
        
        // do the math to find Velocity values to show missile at next
        // location upon screen refresh
        float dx = dMissilePtr->destinationX - xStart; // delta "x" (dx)
        float dy = dMissilePtr->destinationY - yStart; // delta "y" (dy)
        float dist = sqrt(dx*dx + dy*dy);
        dx /= dist;
        dy /= dist;
        float missileVelocityX = 0;
        float missileVelocityY = 0;

        missileVelocityX = game->defMissileSpeed * dx;
        missileVelocityY = missileVelocityY + game->defMissileSpeed * dy;

        // Velocity is a vector quantity that refers to 
        // the rate at which an object changes its position.
        // The diff between X & Y determines the angle
        dMissilePtr->velocity.y = missileVelocityY;
        dMissilePtr->velocity.x = missileVelocityX;

        game->numberDefenseMissiles++;
}