//Author: Jose Garcia, John B. Crossley, Daniel Turack, Jose Reyes
//Program: missileCommand.cpp
//Purpose: This cpp file is the main of our project
//Modified: 5/2/16
//Added comment to prove I can commit several files and merge at once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#ifndef MISSILECOMMAND_H
#define MISSILECOMMAND_H
#include "missileCommand.h"
#endif


#ifndef JOHNC_H
#define JOHNC_H
#include "johnC.h"
#endif


#include "danielT.h"
#include "joseR.h"
#include "joseG.h"
extern "C" {
	#include "fonts.h"
}

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game, Structures *sh);

// JBC added 5/13
void renderDefenseMissile(Game *game);
void makeDefenseMissile(Game *game, int x, int y);

void render(Game *game);

int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	Game game;

	game.numberDefenseMissiles=0;
        
    // JBC 5/19/16
    // added globally accesible defMissileSpeed so that we can 
    // change it dynamically
    game.defMissileSpeed = 80;
	Structures sh;

	//Changed call for function prototype 5-17-16 -DT
	createEMissiles(&game, 0, 0);
	//JR - Menu Object Shapes and Locations
	drawMenu(&game);
	drawSettings(&game);
	//Audio sounds;
	//sounds.loadAudio();
	game.sounds.loadAudio();
	//start animation
	while(!done) {
		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		int state = gameState(&game);
		if (state == 1) {
			renderMenuObjects(&game);
			renderMenuText(&game);
		} else if (state == 2) {
			renderSettings(&game);
		} else {
			movement(&game, &sh);
			render(&game);
		}
		renderStruc(&sh);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "335 Lab1   LMB for dMissile");
}

void cleanupXWindows(void)
{
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void)
{
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
        // JBC just moved code over <- to stay with the "80 lines" requirement
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
			ButtonPress | ButtonReleaseMask |
			PointerMotionMask |
			StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
                InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
	//Initialize Fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}


void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	static int n = 0;
	Audio *a;
	a = &game->sounds;


	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		//LEFT-CLICK
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			//Check game state when LEFT-clicking
			if (gameState(game) == 1 || gameState(game) == 2) {
				a->playAudio(30);
				menuClick(game);
				a->playAudio(32);
			} else {
				// changeTitle();
                makeDefenseMissile(game, e->xbutton.x, y);
                a->playAudio(20);
                // JBC note 5/13
                // moved the "particle" stuff out of here 
				// makeParticle(game, e->xbutton.x, y);
			}
			return;
		}
		//RIGHT-CLICK
		if (e->xbutton.button==3) {
			//Check game state when RIGHT-clicking
			if (gameState(game) == 1) {
				//Menu functions
			} else if (gameState(game) == 0) {
				//Game Functions
				// fireDefenseMissile(game);
                // JBC idea to add menu pop up for right-click
                game->gMenu ^= 1;
			}
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		if (++n < 10)
			return;
		if (gameState(game) == 1) {
			//Menu Functions
			mouseOver(savex, y, game);
		} else if (gameState(game) == 0) {
	        //Game Functions
	        // JBC note 5/13
	        // moved the "particle" stuff out of here 
			// makeParticle(game, e->xbutton.x, y);
		}
	}
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_Escape) {
			return 1;
		}
                
        // Added line for checking "z" key (just closes for now)
		if (key == XK_z) {
			return 1;
		}
		//JR: Allows pause menu if play has been clicked
		if (key == XK_m && game->inGame == 1) {
			game->gMenu ^= 1;
		}

		//You may check other keys here.
	}
	//JR: Check if exit button was clicked
	// This is a temp work around for my exit code and should
	// NOT interfere with anyone elses code or main functions
	if (game->menuExit == 1){
		return 1;
	} else {
		return 0;
	}
}

// JBC note 5/13
// moved the "particle" stuff out of here 
void movement(Game *game, Structures *sh)
{
    // JBC temp comment to see ANYTHING
	eMissilePhysics(game, sh);

	//dMissilePhysics(game, sh);
	eExplosionPhysics(game);
	
	
}

void render(Game *game)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glColor3ub(150,160,220);
	// JBC removed "particle" stuff that is no longer in use   
	// DT
	// JBC commented out... please keep for my testing
	//        if (game->nmissiles < 10) {
	//		createEMissiles(game);
	//	}
    renderEMissiles(game);
	renderEExplosions(game);
    renderDefenseMissile(game);
    // renderDefExplosions(game);
}
