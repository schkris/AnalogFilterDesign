#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

typedef enum class screenState {TITLE_REND, TITLE, TYPE_REND, TYPE, TOPOLOGY_REND, TOPOLOGY, CHARACTERISTICS_REND, CHARACTERISTICS, RESULT_REND, RESULT};

struct fsmRet
{
	screenState currScreen;
	bool exitCon;
};

struct dealocTitRes
{
	TTF_Font* font;
	SDL_Surface* titleSurface;
	SDL_Texture* titleTexture;
	SDL_Surface* instructionsSurface;
	SDL_Texture* instructionsTexture;
};

// Main GUI function called at start up
void openGUI();

// Function Switches between the screens on certain key strokes
fsmRet screenFSM(SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer, screenState currScreen);

// Function draws the title screen
dealocTitRes drawTitle(SDL_Window* window, SDL_Renderer* renderer);
// Function deallocates title allocated vars when done
void deallocateTitleResources(dealocTitRes titResRet);

// Function draws the type select screen
void drawType(SDL_Window* window, SDL_Renderer* renderer, int selectedType);

// Function draws the topology select screen
void drawTopology(SDL_Window* window, SDL_Renderer* renderer, int selectedTopology);

// Function draws a circle
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color);
