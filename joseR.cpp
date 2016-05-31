// Author:   Jose Reyes
// Created:  April 28, 2016
// Modified: May 9, 2016
// Program:  joseR.cpp
// Purpose:  
//			Draw and create a functioning menu that will allow
//			the user to play, change settings, and exit the game.
// Progress:
//		*May 2nd, 2016*
//			Menu shapes, text, and shape color change on mouse hover.
//			The exit button will close on click.
//		*May 9th, 2016*
//			The play button will toggle menu code off and game code on.
//			The settings button will cout a temp message (filler code).
//				This is determined by the function gameState(...)
//			Added a way of treating the menu as a pseudo pause menu.
//				This is determined by "int inGame" in "struct Game"
//		*May 15th, 2016*
//			OpenAL (sound) functionality has been added
//			Must include joseR.h
//		*May 20th, 2016*
//			Moved OpenAL code into a class
//			Changed how others may call the function to play a sound buffer.
//			Requires &game->sounds->playAudio(int);
//			0 = missile_miss, 10 = missile_hit, 20 = missile_fire
//			30/32 = mouse clicks
//		*May 24th, 2016*
//			Finalized audio code (still need in-line comments)
//			Added settings menu (adjust volume for now)
//			Can call sounds via game->sounds.playAudio(int);
//		*May 29th, 2016*
//			Finalized level-to-level code, still need to correctly
//				setup missile and cities alive count.
//			Starting on Game Over screen and High Scores?
//			
#include <GL/glx.h>
#include "missileCommand.h"
#include "joseR.h"
#include <stdio.h>
#include <time.h>
extern "C" {
	#include "fonts.h"
}

using namespace std;

extern void init_opengl();
extern GLuint starsTexture;
extern GLuint cityTexture;
extern void renderBackground(GLuint starsTexture);
extern void renderStruc(Game *game);
extern void renderRadar(Game *game);
extern void renderScores(Game *game);

Audio::Audio()
{
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: alutInit()\n");
		return;
	}
	alGetError();
	device = alcOpenDevice(NULL);
	if (!device) {
		printf("ERROR: device\n");
		return;
	}
	alGetError();
	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		printf("ERROR: context\n");
		return;
	}
	gVolume = 1.0;
	//Setup the listener.
	float vec[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
}

Audio::~Audio()
{
	alDeleteSources(1, &alSource);
	alDeleteBuffers(1, &alBuffer);
	ALCcontext *context = alcGetCurrentContext();
	ALCdevice *device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	alutExit();
}

void Audio::loadAudio()
{
	//Buffer holds the sound information.
	const string FILE[] = {
		"./sounds/missile_explosion.wav", "./sounds/missile_collision.wav",
		"./sounds/missile_launch2.wav", "./sounds/mouse_click.wav",
		"./sounds/mouse_release.wav", "./sounds/classic_tick.wav",
		"./sounds/classic_lvl_start.wav" };
	int val = 0;
	//Load and assign sounds
	for (int i = 0; i < TOTAL_SOUNDS; i++) {
		//Load file into buffer
		const char *f = FILE[i].c_str();
		buffer[i] = alutCreateBufferFromFile(f);
		//Check if anything was loaded into the buffer
		if (!buffer[i]) {
			printf("ERROR: Audio File Not Found!\n");
			printf("[%s] - Was not loaded.\n", f);
			break;
		}
		if (val < 30) {
			//Sets explosions and missile sounds to 10 sources each
			for (int n = 0; n < 10; n++) {
				//Generate a source
				alGenSources(1, &alSource);
				//Store source in a buffer
				alSourcei(alSource, AL_BUFFER, buffer[i]);
				//Set volume, pitch, and loop options
				alSourcef(alSource, AL_GAIN, 1.0f);
				alSourcef(alSource, AL_PITCH, 1.0f);
				alSourcei(alSource, AL_LOOPING, AL_FALSE);
				//Store value of that source to call later
				//printf("File: %s stored in buffer[%d].\n", f, val);
				source[val++] = alSource;
			}
		} else if (val >= 30 && val < 34) {
			//Sets menu click sounds
			for (int n = 0; n < 2; n++) {
				alGenSources(1, &alSource);
				alSourcei(alSource, AL_BUFFER, buffer[i]);
				alSourcef(alSource, AL_GAIN, 1.0f);
				alSourcef(alSource, AL_PITCH, 1.0f);
				alSourcei(alSource, AL_LOOPING, AL_FALSE);
				//printf("File: %s stored in buffer[%d].\n", f, val);
				source[val++] = alSource;
			}
		} else if (val >= 34 && val < 40) {
			//Sets score counter and new level sound
			if (val < 39) {
				for (int n = 0; n < 5; n++) {
					alGenSources(1, &alSource);
					alSourcei(alSource, AL_BUFFER, buffer[i]);
					alSourcef(alSource, AL_GAIN, 1.0f);
					alSourcef(alSource, AL_PITCH, 1.0f);
					alSourcei(alSource, AL_LOOPING, AL_FALSE);
					//printf("File: %s stored in buffer[%d].\n", f, val);
					source[val++] = alSource;
				}
			} else {
				alGenSources(1, &alSource);
				alSourcei(alSource, AL_BUFFER, buffer[i]);
				alSourcef(alSource, AL_GAIN, 1.0f);
				alSourcef(alSource, AL_PITCH, 1.0f);
				alSourcei(alSource, AL_LOOPING, AL_FALSE);
				//printf("File: %s stored in buffer[%d].\n", f, val);
				source[val++] = alSource;
			}
		} else {
			printf("Something may have gone wrong...\n");
		}
	}
}

void Audio::playAudio(int num)
{
	int idx = num, max;
	if (idx < 30) {
		max = idx + 9;
	} else if (idx >= 30 && idx < 34) {
		max = idx +1;
	} else if (idx >= 34 && idx < 49) {
		max = idx + 4;
	} else {
		max = idx;
	}
	alSource = source[idx];
	alGetSourcei(alSource, AL_SOURCE_STATE, &source_state);
	while (source_state == AL_PLAYING) {		
		alSource = source[idx++];
		if (idx > max) {
			printf("Max sources for this sound was reached!\n");
			break;
		}
		alGetSourcei(alSource, AL_SOURCE_STATE, &source_state);
		//printf("Sound already playing!\nPlaying source[%d]\n", idx);
	}
	alSourcef(alSource, AL_GAIN, gVolume);
	alSourcePlay(alSource);
}

void drawMenu(Game *game)
{
	for (int j = 0; j < BUTTONS; j++) {
		game->buttonSpacer[j] = (WINDOW_HEIGHT - 200) -(j*95);
		game->mButton[j].width = 120;
		game->mButton[j].height = 25;
		game->mButton[j].center.x = WINDOW_WIDTH / BUTTON_X;
		game->mButton[j].center.y = WINDOW_HEIGHT - game->buttonSpacer[j];
	}
}

void renderMenuObjects(Game *game)
{
	Shape *s;
	//Check if game is in progress so menu can act as a pause menu.
	//Otherwise will clear screen
	//glClearColor(0.15, 0.15, 0.15, 0.15);
	//glClear(GL_COLOR_BUFFER_BIT);
	float w, h;
	//glBindTexture(GL_TEXTURE_2D, 0);
	for (int i = 0; i < BUTTONS; i++) {
		s = &game->mButton[i];
		glColor3f(0.5, 0.75, 0.90);
		//Button colors based on mouse position
		if (game->mouseOnButton[i] == 1) {
			//Button selected color
			glColor3f(1.0, 0.0, 1.0);
		}
		glPushMatrix();
		glTranslatef(s->center.x, s->center.y, 0);
		w = s->width;
		h = s->height;
		glEnd();
		glBegin(GL_QUADS);
			glVertex2i(-w,-h);
			glVertex2i(-w, h);
			glVertex2i( w, h);
			glVertex2i( w,-h);
		glEnd();
		glPopMatrix();
		//glFlush();
	}
}

void renderMenuText(Game *game)
{
	Rect rt;
	int j = 0;
	rt.bot = WINDOW_HEIGHT - game->buttonSpacer[j] - 10;
	rt.left = WINDOW_WIDTH / BUTTON_X;
	//std::cout << rt.bot << " " << rt.left << std::endl;
	rt.center = 1;
	ggprint16(&rt, 16, 0x00ffffff, "Quit");
	j++;
	rt.bot = WINDOW_HEIGHT - game->buttonSpacer[j] - 10;
	ggprint16(&rt, 16, 0x00ffffff, "Settings");
	j++;
	rt.bot = WINDOW_HEIGHT - game->buttonSpacer[j] - 10;
	if (game->inGame == 0) {
		ggprint16(&rt, 16, 0x00ffffff, "Play");
	} else {
		ggprint16(&rt, 16, 0x00ffffff, "Resume");
	}
}

//Settings buttons draw function goes here

void drawSettings(Game *game)
{
	//
	Shape *s;
	s = &game->menuBG;
	s->width = WINDOW_WIDTH - 650;
	s->height = WINDOW_HEIGHT - 550;
	s->center.x = WINDOW_WIDTH / 2;
	s->center.y = WINDOW_HEIGHT / 2;
	//Back
	s = &game->sButton[0];
	s->width = 125;
	s->height = 25;
	s->center.x = WINDOW_WIDTH / 2;
	s->center.y = WINDOW_HEIGHT - 350;
	//Plus and Minus
	for (int i = 1; i < BUTTONS_S; i++) {
		s = &game->sButton[i];
		s->width = 25;
		s->height = 25;
		if (i == 1)
			s->center.x = WINDOW_WIDTH / 2 + 100;
		if (i == 2)
			s->center.x = WINDOW_WIDTH / 2 - 100;
		s->center.y = WINDOW_HEIGHT - 250;
	}
}

void renderSettings(Game *game)
{
	Shape *s;
	//glClearColor(0.15, 0.15, 0.15, 0.15);
	//glClear(GL_COLOR_BUFFER_BIT);
	//Render Settings Menu BG
	s = &game->menuBG;
	float w, h;
	//glBindTexture(GL_TEXTURE_2D, 0);
	glColor3ub(128,128,128);
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, 0);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	//glFlush();
	//Render Settings Buttons
	for (int i = 0; i < BUTTONS_S; i++) {
		s = &game->sButton[i];
		float w, h;
		glColor3ub(205,92,92);
		if (game->mouseOnButton[i] == 1) {
			//Button selected color
			glColor3ub(190,190,190);
		}
		glPushMatrix();
		glTranslatef(s->center.x, s->center.y, 0);
		w = s->width;
		h = s->height;
		glBegin(GL_QUADS);
			glVertex2i(-w,-h);
			glVertex2i(-w, h);
			glVertex2i( w, h);
			glVertex2i( w,-h);
		glEnd();
		glPopMatrix();
		//glFlush();
	}
}

void renderSettingsText(Game *game)
{
	Rect rt;
	int j = 0;
	rt.bot = WINDOW_HEIGHT - 250 - (j * 100) + 25;
	rt.left = WINDOW_WIDTH / 2;
	//std::cout << rt.bot << " " << rt.left << std::endl;
	rt.center = 1;
	ggprint16(&rt, 16, 0x00ffffff, "Volume");
	rt.bot = WINDOW_HEIGHT - 250 - (j * 100) - 10;
	rt.left = WINDOW_WIDTH / 2 - 100;
	ggprint16(&rt, 16, 0x00ffffff, " - ");
	rt.bot = WINDOW_HEIGHT - 250 - (j * 100) - 10;
	rt.left = WINDOW_WIDTH / 2 + 100;
	ggprint16(&rt, 16, 0x00ffffff, " + ");
	rt.bot = WINDOW_HEIGHT - 250 - (j * 100) - 10;
	rt.left = WINDOW_WIDTH / 2;
	ggprint16(&rt, 16, 0x00ffffff, "%d", game->vVolume);
	j++;
	rt.bot = WINDOW_HEIGHT - 250 - (j * 100) - 10;
	rt.left = WINDOW_WIDTH / 2;
	ggprint16(&rt, 16, 0x00ffffff, "Back");
}

int lvlState(Game *game)
{
	//printf("lvlState()\n");
	if (gameState(game) != 5)
		return -1;
	//TODO: Attach to correct struct variables
	int rMissiles = 10;
	int rCities = 5;
	//Check for Game Over
	if (rCities == 0 && rMissiles == 0) {
		//Set state to some unused value
		game->gState = 10;
		//gameOver(game);
		return 5;
	}
	return 1;
}

void resetLevelEnd(Game *game)
{
    game->lvl.diff = 0;
    game->lvl.cReset = true;
    game->lvl.gtime = 0.0;
    game->lvl.rCount = 0.0;
    game->lvl.cCount = 0.0;
    game->lvl.start = 0;
    game->lvl.end = 0;
    game->lvl.timer = 0.0;
    game->lvl.mDone = 1;
    game->lvl.alertPlayed = 0;
}

void levelEnd(Game *game)
{
	//Variables stored in struct levelInfo
	time_t start = game->lvl.start;
	time_t end   = game->lvl.end;
    double delay = game->lvl.delay;
	float timer = game->lvl.timer;
	int rCount = game->lvl.rCount, rMissiles = 10;
	int cCount = game->lvl.cCount, rCities = 5;
    double diff = game->lvl.diff;
    double m_delay = game->lvl.m_delay;
    double c_delay = game->lvl.c_delay;
	bool clockReset = game->lvl.cReset;
	bool type = 0;
	Audio *a;
	a = &game->sounds;
	if (rCount == rMissiles && game->lvl.mDone == 1)  {
		clockReset = true;
		sleep(1);
	}
	if (clockReset) {
		//printf("Clock Reset\n");
		time(&start);
		timer = 0.0;
		//t = clock();
		clockReset = false;
	}
	//Calculate Score
	if (rCount != rMissiles || cCount != rCities || game->lvl.alertPlayed == 0) {
		timer += 0.1;
		if (rCount != rMissiles) {
			if (timer > m_delay) {
				++rCount;
				a->playAudio(34);			
				glClear(GL_COLOR_BUFFER_BIT);
				renderBackground(starsTexture);
				renderStruc(game);
				renderBonusA(game, rCount, cCount, type);
				//Draw Text/Missile Images
				//Increment Score
				timer = 0.0;
				//t = clock();
			}
		} else if (cCount != rCities) {
			game->lvl.mDone = 0;
			type = 1;
			if (timer > c_delay) {
				++cCount;
				a->playAudio(34);			
				glClear(GL_COLOR_BUFFER_BIT);
				renderBackground(starsTexture);
				renderStruc(game);
				renderBonusA(game, rCount, cCount, type);
				//Draw Text/Missile Images
				//Increment Score
				timer = 0.0;
			}
		} else {
			if (game->lvl.alertPlayed == 0 && difftime(end, start) > 2.0) {
				glClear(GL_COLOR_BUFFER_BIT);
				renderBackground(starsTexture);
				renderStruc(game);
				//renderRadar(game);
				renderNewLevelMsg(game);
				renderScores(game);
				//a->playAudio(39);
				game->lvl.alertPlayed = 1;
			}
		}
	}
	time(&end);
	//Reset Game State once delay is reached
	if (difftime(end, start) >= delay) {
		printf("Level: %d\n", game->level);
		game->gState = 0;
		resetLevelEnd(game);
	} else {
		//Store calculated data
		game->lvl.start = start;
		game->lvl.end = end;
		game->lvl.rCount = rCount;
		game->lvl.cCount = cCount;
		game->lvl.diff = diff;
		game->lvl.cReset = clockReset;
		game->lvl.timer = timer;
	}
}

void renderBonusA(Game *game, int rCount, int cCount, bool type)
{
	int n = rCount;
	int m = cCount;
	//printf("%d | %d\n", n, m);
	Rect rt;
	rt.bot = (WINDOW_HEIGHT / 2) + 100;
	rt.left = (WINDOW_WIDTH / 2) - 100;
	//std::cout << rt.bot << " " << rt.left << std::endl;
	rt.center = 1;
	ggprint16(&rt, 16, 0x00ffffff, "%d", n*5);
	//
	Shape *s;	
	//Missiles
	for (int i = 0; i < n; i++) {
		s = &game->BonusA[i];
		s->width = 2;
		s->height = 5;
		s->center.x = (WINDOW_WIDTH / 2) - 50 + i*10;
		s->center.y = (WINDOW_HEIGHT / 2) + 110;
	}
	for (int i = 0; i < n; i++) {
		s = &game->BonusA[i];
		float w, h;
		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(s->center.x, s->center.y, 0);
		w = s->width;
		h = s->height;
		//Attach texture
		glBindTexture(GL_TEXTURE_2D, cityTexture);
		//For transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
			glVertex2i(-w,-h);
			glVertex2i(-w, h);
			glVertex2i( w, h);
			glVertex2i( w,-h);
		glEnd();
		glDisable(GL_BLEND);
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//Cities
	if (type) {
		Rect rt;
		rt.bot = (WINDOW_HEIGHT / 2) + 50;
		rt.left = (WINDOW_WIDTH / 2) - 105;
		//std::cout << rt.bot << " " << rt.left << std::endl;
		rt.center = 1;
		ggprint16(&rt, 16, 0x00ffffff, "%d", m*100);
		for (int i = 0; i < m; i++) {
			s = &game->BonusB[i];
			s->width = 12;
			s->height = 8;
			s->center.x = (WINDOW_WIDTH / 2) - 50 + i*40;
			s->center.y = (WINDOW_HEIGHT / 2) + 60;
		}
		for (int i = 0; i < m; i++) {
			s = &game->BonusB[i];
			float w, h;
			glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(s->center.x, s->center.y, 0);
			w = s->width;
			h = s->height;
			//Attach texture
			glBindTexture(GL_TEXTURE_2D, cityTexture);
			//For transparency
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glBegin(GL_QUADS);
				glVertex2i(-w,-h);
				glVertex2i(-w, h);
				glVertex2i( w, h);
				glVertex2i( w,-h);
			glEnd();
			glDisable(GL_BLEND);
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void renderNewLevelMsg(Game *game)
{
	Rect rt;
	rt.bot = (WINDOW_HEIGHT / 2);
	rt.left = (WINDOW_WIDTH / 2);
	time_t start = game->lvl.start;
	time_t end   = game->lvl.end;
    double delay = game->lvl.delay;
	//std::cout << rt.bot << " " << rt.left << std::endl;
	rt.center = 1;
	Audio *a;
	a = &game->sounds;
	if (game->lvl.alertPlayed == 0) {
		a->playAudio(39);
		time(&start);
	}
	ggprint16(&rt, 16, 0x00ffffff, "%d X POINTS", game->level);
	rt.bot = (WINDOW_HEIGHT / 2) - 100;
	ggprint16(&rt, 16, 0x00ffffff, "DEFEND               CITIES");
	//
	game->lvl.alertPlayed = 1;
	time(&end);
	if (difftime(end, start) >= delay - 2 && gameState(game) == 3) {
		printf("First Level\n");
		game->gState = 0;
		resetLevelEnd(game);
	} else {
		//Store calculated data
		game->lvl.start = start;
		game->lvl.end = end;
	}
}

void mouseOver(int savex, int savey, Game *game)
{
	Shape *s;
	if (gameState(game) == 1) {
		for (int j = 0; j < BUTTONS; j++) {			
			s = &game->mButton[j];
			if (savey >= s->center.y - (s->height) &&
				savey <= s->center.y + (s->height) &&
				savex >= s->center.x - (s->width) &&
				savex <= s->center.x + (s->width)) {
					game->mouseOnButton[j] = 1;
			} else {
				game->mouseOnButton[j] = 0;
			}
		}
	} else if (gameState(game) == 2) {
		for (int j = 0; j < BUTTONS_S; j++) {			
			s = &game->sButton[j];
			if (savey >= s->center.y - (s->height) &&
				savey <= s->center.y + (s->height) &&
				savex >= s->center.x - (s->width) &&
				savex <= s->center.x + (s->width)) {
					game->mouseOnButton[j] = 1;
			} else {
				game->mouseOnButton[j] = 0;
			}
		}
	}
}

void menuClick(Game *game)
{
	if (gameState(game) == 1) {
		//Play Button (2)
		if (game->mouseOnButton[2] == 1) {
			game->gState = 3;
			game->inGame = 1;
		}
		//Settings Button (1)
		if (game->mouseOnButton[1] == 1) {
			game->gState = 2;
		}
		//Exit Button (0)
		if (game->mouseOnButton[0] == 1) {
			//std::cout << "Quitting..." << std::endl;
			game->menuExit = 1;
		}
	} else if (gameState(game) == 2) {
		float adjust = 0.1;
		int iadjust = 10;
		//Volume -
		if (game->mouseOnButton[2] == 1) {
			if (game->sounds.gVolume > 0.0 && game->sounds.gVolume <= 1.0) {
				game->sounds.gVolume -= adjust;
				game->vVolume -= iadjust;
			}
			//printf("%f\n", game->sounds.gVolume);
		}
		//Volume +
		if (game->mouseOnButton[1] == 1) {
			if (game->sounds.gVolume > -1.0 && game->sounds.gVolume < 1.0) {
				game->sounds.gVolume += adjust;
				game->vVolume += iadjust;
			}
			//printf("%f\n", game->sounds.gVolume);
		}
		//Back
		if (game->mouseOnButton[0] == 1) {
			game->gState = 1;
		}
	}
}

int gameState(Game *game)
{
	int state = 0;
	int g;
	g = game->gState;
	if (g > state) {
		return g;
	}
	return state;
}

float gameVolume(Game *game)
{
	return game->sounds.gVolume;
}
