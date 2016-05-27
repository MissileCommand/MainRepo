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
 *          5/26-27/16
 *          - [x] Make missiles stop at mouse coords 
 *          - [x] Nuke 'em  
 *          - [x] Missiles start higher 

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
        
            
        if (dMissilePtr->destinationY >= dMissilePtr->shape.center.y ) {

//            // test location of Missile vs mouse pick coords
//            cout << "X,Y Missile coords just before the next move (velocity):" << 
//                    dMissilePtr->shape.center.x << 
//                "," << dMissilePtr->shape.center.y << endl; 

            // shape.center refers to the Shape center position
            // IE the position of the center of that particular Shape
            dMissilePtr->shape.center.x += dMissilePtr->velocity.x;
            dMissilePtr->shape.center.y += dMissilePtr->velocity.y;
            
            // Fix to make missiles stop at mouse coords
            // because without this the missile goes 100+ pixels too far
            // I think it has to do with game refresh rate or "render rate"
            if (dMissilePtr->shape.center.y >= dMissilePtr->destinationY) {
                dMissilePtr->shape.center.x = dMissilePtr->destinationX + .01;
                dMissilePtr->shape.center.y = dMissilePtr->destinationY + .01;
                // Added "+ 1" otherwise the missile never explodes 
                        
            }
            
            
        } else {
            
//            // test location of explosion vs mouse pick coords
//            cout << "X,Y Missile coords:" << 
//                    dMissilePtr->shape.center.x << 
//                "," << dMissilePtr->shape.center.y << endl; 
            
            dMissileRemove(game, i);
            
        }

    }
}


void dMissileRemove(Game *game, int dMissilenumber)
{
    DefenseMissile *dMissilePtr = &game->dMissile[dMissilenumber];
    createEExplosion(game,  dMissilePtr->destinationX,
                            dMissilePtr->destinationY);

    //delete defense missile
    game->dMissile[dMissilenumber] = 
        game->dMissile[game->numberDefenseMissiles - 1];
    game->numberDefenseMissiles--;
}

void nukeEmAll (Game *game)
{
    EMissile *enemyMissile;
    
    for (int i=0; i<game->nmissiles; i++) {
        enemyMissile = &game->emarr[i];

	//Use Enemy missile position to create explosion just below it
        if (enemyMissile->vel.x>0) {
            // dowmn and to the right enemy missile
            createEExplosion(game,  
                enemyMissile->pos.x + enemyMissile->vel.x, 
                enemyMissile->pos.y + enemyMissile->vel.y);
                // down to left... vel.y = -0.5 & vel.x -=-0.46
                // down to right. vel.y = -0.5 & vel.x = +0.3
            
        } else { 
            // dowmn and to the left enemy missile
            createEExplosion(game,  
                enemyMissile->pos.x - fabs(enemyMissile->vel.x), 
                enemyMissile->pos.y + enemyMissile->vel.y);
                // down to left... vel.y = -0.5 & vel.x -=-0.46
                // down to right. vel.y = -0.5 & vel.x = +0.3
            
        }
    }
    
    
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
        // do not allow shooting below a certain point 
        dMissilePtr->minimumY = 200;
        if ( y <  dMissilePtr->minimumY ) {
            dMissilePtr->destinationY = dMissilePtr->minimumY;
        } else {
            dMissilePtr->destinationY = y;
            
        }
        
        
        
        
        // test location of explosion vs mouse pick coords
        cout << "X,Y Mouse coords:" << dMissilePtr->destinationX << 
                "," << dMissilePtr->destinationY << endl;
        
        
        
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