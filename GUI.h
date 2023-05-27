#include "Common.h"
#include "FilterDesign.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <mutex>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 1000;
const int DELAY_TIME = 16;

typedef enum class screenState {TITLE_REND, TITLE, TYPE_REND, TYPE, TOPOLOGY_REND, TOPOLOGY, CATEGORY, CATEGORY_REND, CHARACTERISTICS_REND, CHARACTERISTICS, GENERATING_REND, GENERATING, RESULT_REND, RESULT};

typedef enum class charState { GAIN, CUTOFF, STOPBAND, PASSBANDATT, STOPBANDATT, MAXOSC};

struct CharacteristicInputs
{
	std::string gain = "0";
	std::string cutoffFrequency = "0";
	std::string stopbandFrequency = "0";
	std::string passbandAttentuation = "0";
	std::string stopbandAttentuation = "0";
	std::string maxOscillation = "0";
};

struct dealocTitRes
{
	TTF_Font* font;
	SDL_Surface* titleSurface;
	SDL_Texture* titleTexture;
	SDL_Surface* instructionsSurface;
	SDL_Texture* instructionsTexture;
};

struct dealocTypRes
{
	// Not deallocated but needs to be returned
	SDL_Rect submit;
	SDL_Rect back;
	// Rest are deallocated
	TTF_Font* font;
	SDL_Surface* titleSurface;
	SDL_Texture* titleTexture;
	std::vector<SDL_Texture*> filterTextures;
	SDL_Surface* buttonSurface;
	SDL_Texture* buttonTexture;
	SDL_Surface* backSurface;
	SDL_Texture* backTexture;
	SDL_Surface* instructionsSurface;
	SDL_Texture* instructionsTexture;
};

struct dealocCharRes
{
	// Not deallocated but needs to be returned
	SDL_Rect submit;
	SDL_Rect back;
	CharacteristicInputs ui;
	bool confirmShown;
	// Rest are deallocated
	TTF_Font* font;
	SDL_Surface* titleSurface;
	SDL_Texture* titleTexture;
	SDL_Surface* instructionsSurface;
	SDL_Texture* instructionsTexture;
	// 6 Labels
	std::vector<SDL_Surface*> characteristicSurfaces;
	std::vector<SDL_Texture*> characteristicTextures;
	// 6 Values
	SDL_Surface* gainValueSurface;
	SDL_Texture* gainValueTexture;
	SDL_Surface* cutoffValueSurface;
	SDL_Texture* cutoffValueTexture;
	SDL_Surface* stopbandFrequencyValueSurface;
	SDL_Texture* stopbandFrequencyValueTexture;
	SDL_Surface* passbandAttentuationValueSurface;
	SDL_Texture* passbandAttentuationValueTexture;
	SDL_Surface* stopbandAttentuationValueSurface;
	SDL_Texture* stopbandAttentuationValueTexture;
	SDL_Surface* maxOscillationValueSurface;
	SDL_Texture* maxOscillationValueTexture;

	SDL_Surface* buttonSurface;
	SDL_Texture* buttonTexture;
	SDL_Surface* backSurface;
	SDL_Texture* backTexture;
};

struct fsmRet
{
	screenState currScreen;
	bool exitCon;
	typeState typeSel;
	topState topSel;
	catState catSel;
	charState charSel;
	dealocTitRes titRet;
	dealocTypRes typRet;
	dealocTypRes topRet;
	dealocTypRes catRet;
	dealocCharRes charRet;
	bool charSubmitClicked = false;
	bool charChangeMade;
	bool charAttenError;
	bool charFreqError;
};

// Main GUI function called at start up
void openGUI();

// Function Switches between the screens on certain key strokes
fsmRet screenFSM(SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer, fsmRet retFSM);

// Function draws the title screen
dealocTitRes drawTitle(SDL_Window* window, SDL_Renderer* renderer);
// Function deallocates title allocated vars when done
void deallocateTitleResources(dealocTitRes titResRet);

// Function draws the type select screen
dealocTypRes drawType(SDL_Window* window, SDL_Renderer* renderer, int selectedType);
// Function deallocated type allocated vars when done
void deallocateTypeResources(dealocTypRes typResRet);

// Function draws the topology select screen
dealocTypRes drawTopology(SDL_Window* window, SDL_Renderer* renderer, std::vector<std::string> filterTypes, int selectedTopology);

// Function draws the category select screen
dealocTypRes drawCategory(SDL_Window* window, SDL_Renderer* renderer, int selectedType);

// Function draws a circle
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color);

// Function draws the characterisitcs input screen
dealocCharRes drawCharacteristics(SDL_Window* window, SDL_Renderer* renderer, const CharacteristicInputs& inputs, typeState typeSel, catState catSel, charState charSel, std::vector<std::string> filterTypes, bool submitClicked, bool attenError, bool freqError);
// Function deallocates char allocated vars when done
void deallocateCharResources(dealocCharRes charResRet, catState catSel);

// Function draws the generating screen
void drawGenerating(SDL_Window* window, SDL_Renderer* renderer, transferFunct transfer);

// Reads text input from user
void handleTextInputEvent(const SDL_Event& event, std::string& inputText);
