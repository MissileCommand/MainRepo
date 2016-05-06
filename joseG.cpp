//Author: Jose Garcia
//Program: joseG.cpp
//Purpose: This program is my source code for rendering shapes
//that are supposed to represent the floor and cities
//Written: 4/28/16
//Modified: 5/6/16

#include "joseG.h"
#include <GL/glx.h>

extern void init_opengl();

void renderStruc(Structures *shape)
{
	init_opengl();
        //floor shape
        shape->floor.width = WINDOW_WIDTH;
        shape->floor.height = 50;
        shape->floor.center.x = 400;
        shape->floor.center.y = 10;

        //city shape
        for (int i=0; i < CITYNUM; i++) {
                shape->city[i].width = 30;
                shape->city[i].height = 50;
                shape->city[i].center.x = 225 + i*130;
                shape->city[i].center.y = 100;
        }
	float w, h, w2, h2;
        //Draw shapes...

        //draw floor
        glColor3ub(255,255,255);
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
                glColor3ub(87,87,87);
                c = &shape->city[i];
                glPushMatrix();
                glTranslatef(c->center.x - 100, c->center.y, c->center.z);
                w2 = c->width;
                h2 = c->height;
                glBegin(GL_QUADS);
                        glVertex2i(-w2,-h2);
                        glVertex2i(-w2, h2);
                        glVertex2i( w2, h2);
                        glVertex2i( w2,-h2);
                glEnd();
                glPopMatrix();
        }
}
