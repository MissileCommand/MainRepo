//Daniel Turack
//Written: April 27, 2016
//Last Modification Date: May 11, 2016
//Last Modification: Added physics for explosions
//Program will control Enemy Missiles and the resulting collisions


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "danielT.h"
#include "joseR.h"
#include "joseG.h"
extern "C" {
#include "fonts.h"
}

using namespace std;


//macros for vectors
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

//constants
const int MAX_EMISSILES = 10;

int mCount=0;
int chCount=0;
int offCount=0;
int prevLevel=1;
//int level=0;
int redirects=0;

//defined types
typedef float Flt;
typedef Flt Matrix[4][4];
//

//handles missile explosion physics
void eExplosionPhysics(Game *game)
{
	EExplosion *e;

	if (game->neexplosions <=0) {
		return;
	}
	for (int m=0; m<game->neexplosions; m++) {
		e = &game->eearr[m];
		e->radius += e->radiusInc;
		if (rand()%2==0) {
			e->color[0] = 1.0f;
			e->color[1] = 0.1f;
			e->color[2] = 0.1f;
		}
		else if (rand()%3==0) {
			e->color[0] = 1.0f;
			e->color[1] = 1.0f;
			e->color[2] = 0.1f;
		}
		else {
			e->color[0] = 1.0f;
			e->color[1] = 0.549f;
			e->color[2] = 0.0f;
		}

		if (e->radius >= 40.0) {
			e->radiusInc *= -1.25;
		}
		if (e->radius <= 0.0) {
			game->eearr[m] = game->eearr[game->neexplosions-1];
			game->neexplosions--;	
		}
	}
}

//handles missile movement and collisions
void eMissilePhysics(Game *game)
{
	Structures *sh = &game->structures;
	EMissile *e;
	Shape *c;
	EExplosion *d;
	Audio *a;
	a = &game->sounds;

	//if (game->nmissiles <=0)
	//	return;
	for (int i=0; i<game->nmissiles; i++) {
		e = &game->emarr[i];

		//update missile position
		e->pos.x += e->vel.x;
		e->pos.y += e->vel.y;

		//update missile trail
		e->trail.end.x = e->pos.x;
		e->trail.end.y = e->pos.y;

		//check for off screen
		if (e->pos.y < 0.0 || e->pos.x <= 0.0 || e->pos.x >= WINDOW_WIDTH) {
			eMissileExplode(game, i);
			std::cout << "misoff" << std::endl;
			offCount++;
			continue;
		}

		int k;

		//check for collision with cities 
		for (k=0; k<CITYNUM; k++) {
			c = &sh->city[k];
			if (e->pos.y <= c->center.y+c->height && 
					e->pos.x <= c->center.x+c->width && 
					e->pos.x >= c->center.x-c->width) {
				destroyCity(game, k);
				eMissileExplode(game, i);
				a->playAudio(10);
				chCount++;
				break;
			}
		}

		if (k<CITYNUM)
			continue;

		//check for collision with floor
		c = &sh->floor;
		if (e->pos.y <= c->center.y+c->height) {
			eMissileExplode(game, i);
			a->playAudio(0);
			offCount++;
			continue;
		}

		int p;
		//check for DMissile collision
		for (p=0; p<game->neexplosions; p++) {
			d = &game->eearr[p];
			float xd = abs(e->pos.x-d->pos.x);
			float yd = abs(e->pos.y-d->pos.y);
			float dist = sqrt(xd*xd+yd*yd);
			if (dist<=d->radius) {
				eMissileExplode(game,i);
				a->playAudio(0);
				break;
			}
		}
		if (p<game->neexplosions)
			continue;

		//randomly generate new missile branch
		//as long as more missiles are available
		//only generate branches above middle of screen
		if (rand()%100==0 && 
				game->nmissiles<10-game->level && 
				mCount>10-game->level &&
				e->pos.y>WINDOW_HEIGHT/2) {
			for (int q=0; q<(rand()%game->level); q++) {
				createEMissiles(game, e->pos.x, e->pos.y);
			}
		}
	}
	//puts in a random delay between missile creation
	if (game->nmissiles < 10) {
		if (rand()%(60/game->level)==0)
			createEMissiles(game, 0, 0);
	}
	return;
}



void eMissileExplode(Game *game, int misnum)
{
	EMissile *e = &game->emarr[misnum];
	//create explosion graphic
	createEExplosion(game, e->pos.x, e->pos.y);
	//delete missile
	game->emarr[misnum] = game->emarr[game->nmissiles-1];
	game->nmissiles--;
}

void displayScore(Game *g) 
{
	//sleep(10);
	return;
}

//initialize enemy missles from top of screen
void createEMissiles(Game *g, float x, float y)
{
	//counts down missiles in each level
	if (mCount<=0) {
		//waits at the end of each level
		if (g->nmissiles>0 || g->neexplosions>0)
			return;
		g->level++;
		mCount=20*(g->level*0.50);
		//after level has increased return from function 
		//level/score screen will then be displayed
		return;
	}
	//code will eventually be in main
	//////////////////
	if (prevLevel< g->level) {
		displayScore(g);
		prevLevel++;
	}
	/////////////////
	float mRatio;
	EMissile *e = &g->emarr[g->nmissiles];
	if (x==0 && y==0) {
		e->pos.y = WINDOW_HEIGHT-1;    
		e->pos.x = (rand()%WINDOW_WIDTH-1.0) +1.0;
		e->pos.z = 0;
	}
	else {
		e->pos.y = y;
		e->pos.x = x;
		e->pos.z = 0;
	}
	e->vel.y = -1.0*(g->level*0.5);
	//find random destination
	float tempX = rand()%WINDOW_WIDTH;
	//calculate angle using start and end points
	mRatio = (e->pos.x-tempX)/WINDOW_HEIGHT;
	e->vel.x = e->vel.y*mRatio;
	e->vel.z = 0;
	e->color[0] = 0.0f;
	e->color[1] = 0.5f;
	e->color[2] = 0.5f;
	e->trail.start.x = e->pos.x;
	e->trail.start.y = e->pos.y;
	e->trail.start.z = 0.0;
	e->trail.end.x = e->pos.x;
	e->trail.end.y = e->pos.y;
	e->trail.end.z = 0.0;
	if (e->vel.x >= 0)
		e->trail.width = 1.5;
	else
		e->trail.width = -1.5;
	//make trail wider if missile angle is greater
	if (mRatio >= 0.5)
		e->trail.width *= -5.0*mRatio;
	e->trail.color[0] = 0.75;
	e->trail.color[1] = 0.2;
	e->trail.color[2] = 0.2;
	mCount--;
	g->nmissiles++;
}

void createEExplosion(Game *g, float x, float y)
{
	EExplosion *e = &g->eearr[g->neexplosions];
	e->pos.y = y;
	e->pos.x = x;
	e->pos.z = 0;
	e->radius = 4.0;
	e->radiusInc = 0.75;
	e->color[0] = 1.0f;
	e->color[1] = 0.0f;
	e->color[2] = 0.0f;
	g->neexplosions++;
}

void renderEMissiles(Game *g)
{
	if (g->radarOn==0) {
		for (int i=0; i<g->nmissiles; i++) {
			EMissile *e = &g->emarr[i];

			float w = e->trail.width;
			//render missile trails
			glPushMatrix();
			glColor3f(e->trail.color[0], e->trail.color[1], e->trail.color[2]);
			glBegin(GL_QUADS);
			glVertex2i(e->trail.start.x, e->trail.start.y);
			glVertex2i(e->trail.start.x+w, e->trail.start.y+(w*0.5));
			glVertex2i(e->trail.end.x, e->trail.end.y);
			glVertex2i(e->trail.end.x+w, e->trail.end.y+(w*0.5));
			glEnd();
			glPopMatrix();

			//render Missiles
			glPushMatrix();
			glColor3f(e->color[0], e->color[1], e->color[2]);
			glBegin(GL_QUADS);
			glVertex2i(e->pos.x-1, e->pos.y-6);
			glVertex2i(e->pos.x-3, e->pos.y+6);
			glVertex2i(e->pos.x+3, e->pos.y+6);
			glVertex2i(e->pos.x+1, e->pos.y-6);
			glEnd();
			glPopMatrix();
		}
	}
	else {
		for (int i=0; i<g->nmissiles; i++) {
			EMissile *e = &g->emarr[i];

			//render Missiles with radar
			float xd = abs(e->pos.x-g->radar.pos.x);
			float yd = abs(e->pos.y-g->radar.pos.y);
			float dist = sqrt(xd*xd+yd*yd);
			if (dist<=g->radar.radius2 || e->pos.y <=100.0) {
				glPushMatrix();
				glColor3f(e->color[0], e->color[1], e->color[2]);
				glBegin(GL_QUADS);
				glVertex2i(e->pos.x-1, e->pos.y-6);
				glVertex2i(e->pos.x-3, e->pos.y+6);
				glVertex2i(e->pos.x+3, e->pos.y+6);
				glVertex2i(e->pos.x+1, e->pos.y-6);
				glEnd();
				glPopMatrix();
			}
		}
	}
	Rect r;
	//glClear(GL_COLOR_BUFFER_BIT);
	r.bot = WINDOW_HEIGHT-30;
	r.left = 50.0;
	r.center = 0;
	ggprint8b(&r, 16, 0x00005599, "LEVEL: %i", g->level);
	ggprint8b(&r, 16, 0x00005599, "Missiles Remaining: %i", mCount);
	ggprint8b(&r, 16, 0x00005599, "City Hit Count: %i", chCount);
	ggprint8b(&r, 16, 0x00005599, "Missile Miss Count: %i", offCount);
}

//function to be called in main render function to display enemy missiles
void renderEExplosions(Game *g) 
{
	int tris = 20;
	float twicePi = 2.0f * 3.14159265359;
	for (int i=0; i<g->neexplosions; i++) {
		EExplosion *e = &g->eearr[i];
		glPushMatrix();
		glTranslatef(0.0, 0.0, -1.0);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(e->color[0], e->color[1], e->color[2], 0.75);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(e->pos.x, e->pos.y);
		for (int i=0; i<=tris; i++) {
			glVertex2f(
					e->pos.x + (e->radius * cos(i * twicePi/tris)),
					e->pos.y + (e->radius * sin(i * twicePi/tris))
					);
		}
		glDisable(GL_BLEND);
		glEnd();
		glPopMatrix();
	}
}

//DT special feature - radar

void renderRadar(Game *g) 
{
	if (g->radarOn == 0)
		return;
	Radar *r = &g->radar;
	int tris = 100;
	float twicePi = 2.0f * 3.14159265359;
	//glPushMatrix();
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r->color[0], r->color[1], r->color[2], r->color[3]);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(r->pos.x, r->pos.y);
	for (int i=0; i<=tris; i++) {
		glVertex2f(
				r->pos.x + (r->radius * cos(i * twicePi/tris)),
				r->pos.y + (r->radius * sin(i * twicePi/tris))
				);
	}
	glDisable(GL_BLEND);
	glEnd();
	//glPopMatrix();
}

void radarPhysics(Game *g) 
{
	if (g->radarOn == 0)
		return;
	Radar *r = &g->radar;
	if (r->radius2 >= 900.0) {
		initRadar(g);
		return;
	}
	r->radius += r->radiusInc;
	r->radius2 += r->radiusInc;
	//change transparency level
	if ((int)r->radius%10==0)
		r->color[3] = 0.1;
	else
		r->color[3] = 0.3;

}

void initRadar(Game *g) 
{
	Radar *r = &g->radar;
	r->pos.x = WINDOW_WIDTH/2.0;
	r->pos.y = 50.0;
	r->pos.z = 0.0;
	r->radius = 20.0;
	r->radius2 = 30.0;
	r->radiusInc = 5.0;
	r->color[0] = 0.091;
	r->color[1] = 0.973;
	r->color[2] = 0.016;
	r->color[3] = 0.0;
}

void nameInBox(float xpoint, float ypoint)
{
	float w = 50.0;
	float h = 10.0;
	glColor3ub(100, 140, 100);
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex2i(xpoint-w, ypoint-h);
	glVertex2i(xpoint-w, ypoint+h);
	glVertex2i(xpoint+w, ypoint+h);
	glVertex2i(xpoint+w, ypoint-h);
	glEnd();
	glPopMatrix();
	Rect r;    
	r.bot = ypoint - 10;
	r.left = xpoint;
	r.center = 10;
	ggprint8b(&r, 16, 0x00ff0000, "Daniel Turack");
}

