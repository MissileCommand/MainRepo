#ifndef _JOSER_H_
#define _JOSER_H_

//Function Prototypes
extern void drawMenu(Game *game);
extern void mouseOver(int x, int y, Game *game);
extern void renderMenuObjects(Game *game);
extern void renderMenuText(Game *game);
extern void menuClick(Game *game);
extern int gameState(Game *game);
extern void init_openal();
extern void playSound(Game *game, int n);
extern void cleanup_openal(Game *game);

#endif