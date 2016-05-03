//cs335 Spring 2015 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//




//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "missileCommand.h"
#include "johnBC.h"
#include "danielT.h"
#include "joseR.h"

// JG go through this to remove the redefs
// #include "joseG.h"



//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

/*
 * These are already in main file "missileCommand"
 * Had to pull out to prevent compile errors
 
 */




//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	Game game;
	game.n=0;

	//declare a box shape
	game.box.width = 100;
	game.box.height = 10;
	game.box.center.x = 120 + 5*65;
	game.box.center.y = 500 - 5*60;
	
	//function call to danielt.cpp for creation of enemy missiles
	createEMissiles(&game);

	//start animation
	while(!done) {
		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e, &game);
			done = check_keys(&e, &game);
		}
		movement(&game);
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void) {
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
	if(vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
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
}

void makeParticle(Game *game, int x, int y) {
	if (game->n >= MAX_PARTICLES)
		return;
	//std::cout << "makeParticle()" << x << " " << y << std::endl;
	//position of particle
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = -4.0;
	p->velocity.x =  1.0;
	game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
	static int savex = 0;
	static int savey = 0;
	static int n = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			makeParticle(game, e->xbutton.x, y);
                        changeTitle();
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
                        fireDefenseMissile();
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		if (++n < 10)
			return;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		makeParticle(game, e->xbutton.x, y);


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
		//You may check other keys here.

	}
	return 0;
}

void movement(Game *game)
{
	Particle *p;

	if (game->n <= 0)
		return;

	for (int i=0; i<game->n; i++) {
		p = &game->particle[i];
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;

		//gravity
		p->velocity.y -= 0.2;

		//check for collision with shapes...
		Shape *s;
		s = &game->box;
		if (p->s.center.y >= s->center.y - (s->height) &&
		    p->s.center.y <= s->center.y + (s->height) &&
		    p->s.center.x >= s->center.x - (s->width) &&
		    p->s.center.x <= s->center.x + (s->width)) {
			p->velocity.y *= -1.0;
		}

		//check for off-screen
		if (p->s.center.y < 0.0) {
			std::cout << "off screen" << std::endl;
                        //fireDefenseMissile();
			game->particle[i] = game->particle[game->n-1];
			game->n--;
		}
	}
}

void render(Game *game)
{
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...

	//draw box
	Shape *s;
	glColor3ub(90,140,90);
	s = &game->box;
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

	//draw all particles here
	glPushMatrix();
	glColor3ub(150,160,220);
	for (int i=0; i<game->n; i++) {
		Vec *c = &game->particle[i].s.center;
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
	//function call to danielt.cpp to render enemy missiles
	renderEMissiles(game);
}



//////////******************/////////////////////////
//////////******************/////////////////////////
//////////******************/////////////////////////
//////////******************/////////////////////////
//////////******************/////////////////////////
//////////******************/////////////////////////
//////////******************/////////////////////////
//////////******************/////////////////////////

// added in "main.cpp" for JG code

//function prototypes
void initXWindows(void);
void initOpengl(void);
void cleanupXWindows(void);
void checkResize(XEvent *e);
void checkMouse(XEvent *e);
void checkKeys(XEvent *e);
void render(Structures *shape);

int done=0;
int xres=1024, yres=768;


// JBC commented out for compile
//Ppmimage *forestImage=NULL;
//GLuint forestTexture;
//int forest=1;


// JBC commented out for compile
//int main(void)
//{
//	logOpen();
//	initXWindows();
//	initOpengl();
//	Structures shape;
//	while(!done) {
//		while(XPending(dpy)) {
//			XEvent e;
//			XNextEvent(dpy, &e);
//			checkResize(&e);
//			checkMouse(&e);
//			checkKeys(&e);
//		}
//		//Always render every frame.
//		render(&shape);
//		glXSwapBuffers(dpy, win);
//	}
//	cleanupXWindows();
//	cleanup_fonts();
//	logClose();
//	return 0;
//}

//void cleanupXWindows(void)
//{
//	XDestroyWindow(dpy, win);
//	XCloseDisplay(dpy);
//}

void setTitle(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "CS335 - OpenGL Animation Template Under XWindows");
}

void setupScreenRes(const int w, const int h)
{
	xres = w;
	yres = h;
}

//void initXWindows(void)
//{
//	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
//	//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
//	XSetWindowAttributes swa;
//
//	setupScreenRes(1024, 768);
//	dpy = XOpenDisplay(NULL);
//	if (dpy == NULL) {
//		printf("\n\tcannot connect to X server\n\n");
//		exit(EXIT_FAILURE);
//	}
//	Window root = DefaultRootWindow(dpy);
//	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
//	if (vi == NULL) {
//		printf("\n\tno appropriate visual found\n\n");
//		exit(EXIT_FAILURE);
//	} 
//	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
//	swa.colormap = cmap;
//	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
//						StructureNotifyMask | SubstructureNotifyMask;
//	win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
//							vi->depth, InputOutput, vi->visual,
//							CWColormap | CWEventMask, &swa);
//	GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
//	glXMakeCurrent(dpy, win, glc);
//	setTitle();
//}

void reshapeWindow(int width, int height)
{
	//window has been resized.
	setupScreenRes(width, height);
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, xres, 0, yres, -1, 1);
	setTitle();
}

//unsigned char *buildAlphaData(Ppmimage *img)
//{
//	//add 4th component to RGB stream...
//	int i;
//	int a,b,c;
//	unsigned char *newdata, *ptr;
//	unsigned char *data = (unsigned char *)img->data;
//	newdata = (unsigned char *)malloc(img->width * img->height * 4);
//	ptr = newdata;
//	for (i=0; i<img->width * img->height * 3; i+=3) {
//		a = *(data+0);
//		b = *(data+1);
//		c = *(data+2);
//		*(ptr+0) = a;
//		*(ptr+1) = b;
//		*(ptr+2) = c;
//		//get largest color component...
//		//*(ptr+3) = (unsigned char)((
//		//		(int)*(ptr+0) +
//		//		(int)*(ptr+1) +
//		//		(int)*(ptr+2)) / 3);
//		//d = a;
//		//if (b >= a && b >= c) d = b;
//		//if (c >= a && c >= b) d = c;
//		//*(ptr+3) = d;
//		*(ptr+3) = (a|b|c);
//		ptr += 4;
//		data += 3;
//	}
//	return newdata;
//}

void initOpengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, xres, yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);

	glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	
        //JBC comment out for compile
        // initialize_fonts();
	//
	//load the images file into a ppm structure.
	//

        //JBC comment out for compile
        // 	forestImage      = ppm6GetImage("stars.ppm");
	//
	//create opengl texture elements
        
	//JBC comment out for compile
        // glGenTextures(1, &forestTexture);
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//
	//forest
	
        //JBC comment out for compile
        // glBindTexture(GL_TEXTURE_2D, forestTexture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	
        //JBC comment out for compile
        // glTexImage2D(GL_TEXTURE_2D, 0, 3,
	//JBC comment out for compile
        // 						forestImage->width, forestImage->height,
	//JBC comment out for compile
        // 						0, GL_RGB, GL_UNSIGNED_BYTE, forestImage->data);
	//-------------------------------------------------------------------------
}
void checkResize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != xres || xce.height != yres) {
		//Window size did change.
		reshapeWindow(xce.width, xce.height);
	}
}

void checkMouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
	}
}

void checkKeys(XEvent *e)
{
	//keyboard input?
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress) {
		if (key == XK_Escape) {
			done = 1;
		}
		if (key == XK_f) {
	//JBC comment out for compile
        // forest ^=1;
		}

	}
}

void render(Structures *shape)
{
	//JBC comment out for compile
        // Rect r;

	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//
	//
	//draw a quad with texture
	//glColor3f(1.0, 1.0, 1.0);
	/*iif (forest) {
		glBindTexture(GL_TEXTURE_2D, forestTexture);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
			glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
			glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
			glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
		glEnd();
	}
	//
	//
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0, "F - Forest");
	*/
	//JBC comment out for compile
        // renderStruc(shape);
}
