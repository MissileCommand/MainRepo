#ifndef _JOSER_H_
#define _JOSER_H_

//Function Prototypes
extern void drawMenu(Game *game);
extern void mouseOver(int x, int y, Game *game);
extern void renderMenuObjects(Game *game);
extern void renderMenuText(Game *game);
extern void menuClick(Game *game);
extern int gameState(Game *game);
extern int initDevice(ALCdevice** device);
extern void playSound(Game *game, int n);

class Audio 
{
	public:
		//Variables
		ALCdevice *device;
		ALCcontext *context;
		ALuint alSource;
		ALuint alBuffer;
		ALint source_state;
		int source[20];
		int buffer[4];
		//Constructor & Deconstructor
		Audio();
		~Audio();
		//Class Prototypes
		void loadAudio();
		void playAudio(int);
};

#endif