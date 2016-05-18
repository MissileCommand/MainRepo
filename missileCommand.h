/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   missileCommand.h
 * Author: student
 *
 * Created on April 28, 2016, 3:52 PM
 */

// #ifndef MISSILECOMMAND_H
// #define MISSILECOMMAND_H

//INCLUDES
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <string>
#include <cmath>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

//MACROS
//Number of buttons to show on menu
#define BUTTONS 3
//X Position of all buttons
//Divides WINDOW_WIDTH by the number below
#define BUTTON_X 4.25
#define CITYNUM 5
//#define WINDOW_WIDTH  800
//#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768
#define MAX_D_MISSILES 100
#define GRAVITY 0.1

//X Windows variables
extern Display *dpy;
extern Window win;
extern GLXContext glc;

struct Vec {
    float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};
  
// JBC 05/08/16 JBC switched from DefenseMissile to dMissile/DefenseMissile
//struct DefenseMissile {
//	Shape s;
//	Shape s2;
//	Shape s3;
//	Vec velocity;
//};
struct DefenseMissile {
	Shape s;
	Shape s2;
	Shape s3;
	Vec velocity;
};

struct Structures {
        Shape floor;
        Shape city[CITYNUM];
}; 

struct EMTrail {
    Vec start;
    Vec end;
    float width;
    float color[3];
    EMTrail() {}
};

struct EMissile {
    Vec pos;
    Vec vel;
    float angle;
    float color[3];        
    EMTrail trail;
    EMissile() { }
};

struct EExplosion {
    Vec pos;
    float radius; 
    float radiusInc;
    float color[3];
    EExplosion() {}
};

struct gameSound {
    int source[4];
    int buffer[4];
    int max;
    gameSound() {}
};

struct Game {
    Shape box;    
    // JBC comment out
    // Particle particle[10];
    EMissile *emarr;                                  
    int n;
    int nmissiles;
    EExplosion *eearr;
    int neexplosions;
    
    // JBC 05/08/16 JBC switched from DefenseMissile to dMissile (Defense Missile)
    DefenseMissile dMissile[MAX_D_MISSILES];

    
    //JR: This will store the values of the x-pos to be used
    //      for correct font placement on buttons
    int buttonSpacer[BUTTONS];
    //JR: This will be used to return which button the mouse
    //      is currently on in the menus
    int mouseOnButton[BUTTONS];
    //JR
    int menuExit, gMenu, inGame;
    Shape mButton[BUTTONS];
    gameSound sound;
    //Constructor 
    Game() {
    	emarr = new EMissile[10];
        eearr = new EExplosion[100];
    	n = 0;
    	nmissiles = 0;
        neexplosions = 0;
        menuExit = 0;
        gMenu = 1;
        inGame = 0;
        for (int i=0;i<BUTTONS;i++) {
            mouseOnButton[i] = 0;
        }
    }
    //Deconstructor
    ~Game() {
	   delete [] emarr;
	   delete [] eearr;
    }    
};


#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

// #endif /* MISSILECOMMAND_H */

