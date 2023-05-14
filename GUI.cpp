#include "GUI.h"

const char* WINDOW_TITLE = "Analog Filter Designer";

void openGUI()
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // Create a window and renderer
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create FSM return struct
    fsmRet retFSM = {screenState::TITLE_REND ,false};

    // Run the event loop
    while (!retFSM.exitCon)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            retFSM = screenFSM(&event, window, renderer, retFSM.currScreen);
        }

        SDL_Delay(16); // Sleep for a short time to avoid hogging the CPU
    }

    // Clean up and exit
    SDL_DestroyWindow(window);
    SDL_Quit();

}


fsmRet screenFSM(SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer, screenState currScreen)
{
    fsmRet ret;
    dealocTitRes titResRet = {};
    if (event->type == SDL_QUIT)
    {
        ret.exitCon = true;
        ret.currScreen = currScreen;
        return ret;
    }
    else
    {
        // Set background color to white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Switch statement between Screens
        switch (currScreen)
        {
            case screenState::TITLE_REND:
                // Title Screen Render
                titResRet = drawTitle(window, renderer);
                currScreen = screenState::TITLE;
            break;

            case screenState::TITLE:
                // Title Screen Static
                if (event->type == SDL_MOUSEBUTTONUP)
                {
                    deallocateTitleResources(titResRet);
                    currScreen = screenState::TYPE_REND;
                }
                break;

            case screenState::TYPE_REND:
                // Filter Type Render (Low, High, Band, Notch)
                drawType(window, renderer, 1);
                currScreen = screenState::TYPE;
            break;

            case screenState::TYPE:
                // Filter Type Static (Low, High, Band, Notch)
                if (event->type == SDL_MOUSEBUTTONUP)
                {
                    currScreen = screenState::TOPOLOGY_REND;
                }
            break;

            case screenState::TOPOLOGY_REND:
                // Filter Topology Render (MFB, Sallen - Key)
                drawTopology(window, renderer, 2);
                currScreen = screenState::TOPOLOGY;
            break;

            case screenState::TOPOLOGY:

            break;

            case screenState::CHARACTERISTICS:
                // Filter Characteristics(Gain, Cut Off)
            break;

            case screenState::RESULT:
                // Results
            break;

            default:

            break;
        }
        ret.exitCon = false;
        ret.currScreen = currScreen;
        return ret;
    }

}


dealocTitRes drawTitle(SDL_Window* window, SDL_Renderer* renderer)
{
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", 24);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Filter Design Tool", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create instructions text surface and texture
    SDL_Surface* instructionsSurface = TTF_RenderText_Solid(font, "Click to Continue", textColor);
    SDL_Texture* instructionsTexture = SDL_CreateTextureFromSurface(renderer, instructionsSurface);

    // Get window dimensions
    int winWidth, winHeight;
    SDL_GetWindowSize(window, &winWidth, &winHeight);

    // Get title texture dimensions
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    // Get instructions texture dimensions
    int instructionsWidth, instructionsHeight;
    SDL_QueryTexture(instructionsTexture, NULL, NULL, &instructionsWidth, &instructionsHeight);

    // Draw title texture in the center of the screen
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 50, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Draw instructions texture in the center of the screen below the title
    SDL_Rect instructionsRect = { (winWidth - instructionsWidth) / 2, (winHeight - instructionsHeight) / 2 + 50, instructionsWidth, instructionsHeight };
    SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

    // Assign resources that must be deallocated
    dealocTitRes resRet;
    resRet.font = font;
    resRet.instructionsSurface = instructionsSurface;
    resRet.instructionsTexture = instructionsTexture;
    resRet.titleSurface = titleSurface;
    resRet.titleTexture = titleTexture;

    // Present the renderer
    SDL_RenderPresent(renderer);

    return resRet;
}

void deallocateTitleResources(dealocTitRes titResRet)
{
    TTF_CloseFont(titResRet.font);
    SDL_FreeSurface(titResRet.titleSurface);
    SDL_DestroyTexture(titResRet.titleTexture);
    SDL_FreeSurface(titResRet.instructionsSurface);
    SDL_DestroyTexture(titResRet.instructionsTexture);
}


void drawType(SDL_Window* window, SDL_Renderer* renderer, int selectedType)
{
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("RobotoMono-VariableFont_wght.ttf", 24);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Select Filter Type", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Get window dimensions
    int winWidth, winHeight;
    SDL_GetWindowSize(window, &winWidth, &winHeight);

    // Get title texture dimensions
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    // Draw title texture in the center of the screen
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Create list of filter types
    std::vector<std::string> filterTypes = { "Low Pass", "High Pass", "Band Pass", "Notch" };

    // Get filter type texture dimensions
    int maxFilterWidth = 0, filterHeight = 0;
    std::vector<int> filterWidths;
    std::vector<SDL_Texture*> filterTextures;
    for (const auto& type : filterTypes)
    {
        SDL_Surface* filterSurface = TTF_RenderText_Solid(font, type.c_str(), textColor);
        SDL_Texture* filterTexture = SDL_CreateTextureFromSurface(renderer, filterSurface);
        filterTextures.push_back(filterTexture);
        int filterWidth;
        SDL_QueryTexture(filterTexture, NULL, NULL, &filterWidth, &filterHeight);
        filterWidths.push_back(filterWidth);
        if (filterWidth > maxFilterWidth) {
            maxFilterWidth = filterWidth;
        }
    }

    // Draw filter type textures in a list
    int startY = (winHeight - filterTypes.size() * filterHeight) / 2;
    int startX = (winWidth - maxFilterWidth) / 2;
    for (size_t i = 0; i < filterTypes.size(); i++)
    {
        int filterWidth = filterWidths[i];
        int xPos = 350;
        SDL_Rect filterRect = { xPos, startY + i * filterHeight, filterWidth, filterHeight };
        SDL_RenderCopy(renderer, filterTextures[i], NULL, &filterRect);
    }

    // Highlight the selected filter type
    SDL_Color color = { 0, 0, 0, 127 };
    int xPos = (winWidth - maxFilterWidth - 20) / 2;
    int yPos = startY + selectedType * filterHeight - 10;
    drawCircle(renderer, xPos, yPos, 5, color);


    // Create submit button texture
    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Submit", textColor);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);

    // Get button texture dimensions
    int buttonWidth, buttonHeight;
    SDL_QueryTexture(buttonTexture, NULL, NULL, &buttonWidth, &buttonHeight);

    // Draw button texture at bottom center of screen
    SDL_Rect buttonRect = { (winWidth - buttonWidth) / 2, winHeight - buttonHeight - 50, buttonWidth, buttonHeight };
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Present the renderer
    SDL_RenderPresent(renderer);

    // Clean up textures
    for (auto& texture : filterTextures)
    {
        SDL_DestroyTexture(texture);
    }

    // Clean up font
    TTF_CloseFont(font);
}

void drawTopology(SDL_Window* window, SDL_Renderer* renderer, int selectedTopology)
{
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", 24);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Select Topology", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Get window dimensions
    int winWidth, winHeight;
    SDL_GetWindowSize(window, &winWidth, &winHeight);

    // Get title texture dimensions
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    // Draw title texture in the center of the screen
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Create list of topology options
    std::vector<std::string> topologyOptions = { "Multifeedback", "Sallen-Key" };

    // Get topology option texture dimensions
    int optionWidth = 0, optionHeight = 0;
    std::vector<SDL_Texture*> optionTextures;
    for (const auto& option : topologyOptions)
    {
        SDL_Surface* optionSurface = TTF_RenderText_Solid(font, option.c_str(), textColor);
        SDL_Texture* optionTexture = SDL_CreateTextureFromSurface(renderer, optionSurface);
        optionTextures.push_back(optionTexture);
        SDL_QueryTexture(optionTexture, NULL, NULL, &optionWidth, &optionHeight);
    }

    // Draw topology option textures in a list
    int startY = (winHeight - topologyOptions.size() * optionHeight) / 2;
    for (size_t i = 0; i < topologyOptions.size(); i++)
    {
        SDL_Rect optionRect = { (winWidth - optionWidth) / 2, startY + i * optionHeight, optionWidth, optionHeight };
        SDL_RenderCopy(renderer, optionTextures[i], NULL, &optionRect);
    }

    // Highlight the selected topology option
    SDL_Rect selectionRect = { (winWidth - optionWidth - 20) / 2, startY + selectedTopology * optionHeight - 10, optionWidth + 20, optionHeight + 20 };
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &selectionRect);

    // Draw button texture at bottom center of screen
    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Submit", textColor);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    int buttonWidth, buttonHeight;
    SDL_QueryTexture(buttonTexture, NULL, NULL, &buttonWidth, &buttonHeight);
    SDL_Rect buttonRect = { (winWidth - buttonWidth) / 2, winHeight - buttonHeight - 50, buttonWidth, buttonHeight };
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Present the renderer
    SDL_RenderPresent(renderer);

    // Clean up textures
    for (auto& texture : optionTextures)
    {
        SDL_DestroyTexture(texture);
    }
    SDL_DestroyTexture(buttonTexture);

    // Clean up font
    TTF_CloseFont(font);
}



void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color)
{
    // Set the render draw color to the desired color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the circle
    for (int x = centerX - radius; x <= centerX + radius; x++) {
        for (int y = centerY - radius; y <= centerY + radius; y++) {
            if ((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

