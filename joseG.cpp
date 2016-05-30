//Author: Jose Garcia
//Program: joseG.cpp
//Purpose: This program is my source code for rendering shapes
//that are supposed to represent the floor and cities
//Written: 4/28/16
//Modified: 5/6/16

#include "joseG.h"
#include <GL/glx.h>
extern "C" {
#include "fonts.h"
}

//extern void init_opengl();

GLuint cityTexture;


void initStruc(Game *game)
{

	Structures *shape = &game->structures;
	//floor shape
	shape->floor.width = WINDOW_WIDTH;
	shape->floor.height = 50;
	shape->floor.center.x = 400;
	shape->floor.center.y = 10;

	//city shape
	for (int i=0; i < CITYNUM; i++) {
		shape->city[i].alive = 1;
		shape->city[i].width = 30;
		shape->city[i].height = 50;
		shape->city[i].center.x = 225 + i*130;
		shape->city[i].center.y = 100;
	}
}

void destroyCity(Game *game, int citynum)
{
	Shape *dcity = &game->structures.city[citynum];
	dcity->alive = 0;
	return;
}

void renderStruc(Game *game)
{
	Structures *shape = &game->structures;

	//Draw shapes...
	float w, h, w2, h2;

	//draw floor
	glColor3ub(105,105,105);
	Shape *s;
	s = &shape->floor;
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

	//draw cities
	Shape *c;
	for (int i = 0; i < CITYNUM; i++) {
		c = &shape->city[i];
		if (c[i].alive) {
		glColor3i(1, 1, 1);
		//or
		//glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(c->center.x, c->center.y, c->center.z);
		w2 = c->width;
		h2 = c->height;
		glBindTexture(GL_TEXTURE_2D, cityTexture);
		//For transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//glColor4ub(255,255,255,255);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 1.0f); glVertex2i(-w2,-h2);
			glTexCoord2f(1.0f, 0.0f); glVertex2i(-w2, h2);
			glTexCoord2f(0.0f, 0.0f); glVertex2i( w2, h2);
			glTexCoord2f(0.0f, 1.0f); glVertex2i( w2,-h2);
		glEnd();
		glDisable(GL_BLEND);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void renderBackground(GLuint starsTexture)
{
	glBindTexture(GL_TEXTURE_2D, starsTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, WINDOW_HEIGHT);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(WINDOW_WIDTH, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void renderScores(Game *game)
{
    Rect r;
    r.bot = 30;
    r.left = 50.0;
    r.center = 0;
    ggprint16(&r, 16, 0x00005599, "Score: %i", game->score);
}
