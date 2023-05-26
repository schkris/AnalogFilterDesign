#include "GUI.h"

const char* WINDOW_TITLE = "Analog Filter Designer";
commonVars shared; // Define the shared variable

void openGUI() 
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    if (TTF_Init() == -1) {
        std::cerr << "Failed to initialize SDL2 TTF: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create SDL2 window: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    fsmRet retFSM = { 
        screenState::TITLE_REND,
        false,
        typeState::LOW,
        topState::MFB,
        catState::BUTT,
        charState::GAIN,
        {},
        {},
        {},
        {},
        false,
        false
    };

    bool eventProcessing = false;
    while (!retFSM.exitCon) 
    {
        SDL_Event event;
        SDL_Event eventQueue[10]; SDL_PeepEvents(eventQueue, 10, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
        while (!eventProcessing && SDL_PollEvent(&event))
        {
            eventProcessing = true;
            retFSM = screenFSM(&event, window, renderer, retFSM);
            eventProcessing = false;
            SDL_Delay(DELAY_TIME); // Sleep for a short time to avoid hogging the CPU
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}


fsmRet screenFSM(SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer, fsmRet retFSM)
{
    if (event->type == SDL_QUIT)
    {
        retFSM.exitCon = true;
        return retFSM;
    }
    else
    {
        // Set background color to white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Switch statement between Screens
        switch (retFSM.currScreen)
        {
            case screenState::TITLE_REND:
                // Title Screen Render
                retFSM.titRet = drawTitle(window, renderer);
                retFSM.currScreen = screenState::TITLE;
            break;

            case screenState::TITLE:
                // Title Screen Static
                if (event->type == SDL_MOUSEBUTTONUP)
                {
                    deallocateTitleResources(retFSM.titRet);
                    retFSM.currScreen = screenState::TYPE_REND;
                }
                break;

            case screenState::TYPE_REND:
                // Filter Type Render (Low, High, Band, Notch)
                if (retFSM.typeSel == typeState::LOW)
                {
                    retFSM.typRet = drawType(window, renderer, 1);
                }
                else if (retFSM.typeSel == typeState::HIGH)
                {
                    retFSM.typRet = drawType(window, renderer, 2);
                }
                else if (retFSM.typeSel == typeState::BAND)
                {
                    retFSM.typRet = drawType(window, renderer, 3);
                }
                else if(retFSM.typeSel == typeState::NOTCH)
                {
                    retFSM.typRet = drawType(window, renderer, 4);
                }
                else if(retFSM.typeSel == typeState::STATE)
                {
                    retFSM.typRet = drawType(window, renderer, 5);
                }
                retFSM.currScreen = screenState::TYPE;
            break;

            case screenState::TYPE:
                // Filter Type Static (Low, High, Band, Notch, State)
                switch (retFSM.typeSel)
                {
                    case typeState::LOW:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::HIGH;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                    break;

                    case typeState::HIGH:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::LOW;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::BAND;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                    break;

                    case typeState::BAND:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::HIGH;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::NOTCH;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                    break;

                    case typeState::NOTCH:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::BAND;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::STATE;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                    break;

                    case typeState::STATE:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.typeSel = typeState::NOTCH;
                            retFSM.currScreen = screenState::TYPE_REND;
                        }
                    break;
                    
                    default:
                    break;
                }
                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.typRet.submit.x &&
                    event->button.x <= (retFSM.typRet.submit.x + retFSM.typRet.submit.w) &&
                    event->button.y >= retFSM.typRet.submit.y &&
                    event->button.y <= (retFSM.typRet.submit.y + retFSM.typRet.submit.h))
                {
                    // Submit button clicked
                    deallocateTypeResources(retFSM.typRet);
                    shared.type = retFSM.typeSel;
                    if(retFSM.typeSel == typeState::STATE)
                    {
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else
                    {
                        retFSM.currScreen = screenState::TOPOLOGY_REND;
                    }
                }

                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.typRet.back.x &&
                    event->button.x <= (retFSM.typRet.back.x + retFSM.typRet.back.w) &&
                    event->button.y >= retFSM.typRet.back.y &&
                    event->button.y <= (retFSM.typRet.back.y + retFSM.typRet.back.h))
                {
                    // Back button clicked
                    deallocateTypeResources(retFSM.typRet);
                    shared.type = retFSM.typeSel;
                    retFSM.currScreen = screenState::TITLE_REND;
                }

            break;

            case screenState::TOPOLOGY_REND:
                // Filter Topology Render IF(NOTCH):(TWIN-T, MFB, Sallen-Key, STATE_VAR) ELSE:(MFB, Sallen - Key, State Variable)
                if(retFSM.typeSel == typeState::NOTCH)
                {
                    if (retFSM.topSel == topState::TWIN)
                    {
                        retFSM.topRet = drawTopology(window, renderer, { "Twin-T", "Multifeedback", "Sallen-Key"},1);
                    }
                    else if (retFSM.topSel == topState::MFB)
                    {
                        retFSM.topRet = drawTopology(window, renderer, { "Twin-T", "Multifeedback", "Sallen-Key"},2);
                    }
                    else if (retFSM.topSel == topState::SK)
                    {
                        retFSM.topRet = drawTopology(window, renderer, { "Twin-T", "Multifeedback", "Sallen-Key"},3);
                    }
                }
                else
                {
                    if(retFSM.topSel == topState::TWIN)
                    {
                        retFSM.topSel = topState::MFB;
                    }
                    if (retFSM.topSel == topState::MFB)
                    {
                        retFSM.topRet = drawTopology(window, renderer, { "Multifeedback", "Sallen-Key"},1);
                    }
                    else if (retFSM.topSel == topState::SK)
                    {
                        retFSM.topRet = drawTopology(window, renderer, { "Multifeedback", "Sallen-Key"},2);
                    }
                }
                retFSM.currScreen = screenState::TOPOLOGY;
            break;

            case screenState::TOPOLOGY:
                switch (retFSM.topSel)
                {
                    case topState::TWIN:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.topSel = topState::MFB;
                            retFSM.currScreen = screenState::TOPOLOGY_REND;
                        }
                    break;

                    case topState::MFB:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED && retFSM.typeSel == typeState::NOTCH)
                        {
                            retFSM.topSel = topState::TWIN;
                            retFSM.currScreen = screenState::TOPOLOGY_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.topSel = topState::SK;
                            retFSM.currScreen = screenState::TOPOLOGY_REND;
                        }
                    break;

                    case topState::SK:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.topSel = topState::MFB;
                            retFSM.currScreen = screenState::TOPOLOGY_REND;
                        }
                    break;

                    default:

                    break;
                }
                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.topRet.submit.x &&
                    event->button.x <= (retFSM.topRet.submit.x + retFSM.topRet.submit.w) &&
                    event->button.y >= retFSM.topRet.submit.y &&
                    event->button.y <= (retFSM.topRet.submit.y + retFSM.topRet.submit.h))
                {
                    // Submit button clicked
                    deallocateTypeResources(retFSM.topRet);
                    shared.topography = retFSM.topSel;
                    retFSM.currScreen = screenState::CATEGORY_REND;
                }

                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.topRet.back.x &&
                    event->button.x <= (retFSM.topRet.back.x + retFSM.topRet.back.w) &&
                    event->button.y >= retFSM.topRet.back.y &&
                    event->button.y <= (retFSM.topRet.back.y + retFSM.topRet.back.h))
                {
                    // Back button clicked
                    deallocateTypeResources(retFSM.topRet);
                    shared.topography = retFSM.topSel;
                    retFSM.currScreen = screenState::TYPE_REND;
                }
            break;

            case screenState::CATEGORY_REND:
                // Filter Type Render (Low, High, Band, Notch)
                if (retFSM.catSel == catState::BUTT)
                {
                    retFSM.catRet = drawCategory(window, renderer, 1);
                }
                else if (retFSM.catSel == catState::CHEB1)
                {
                    retFSM.catRet = drawCategory(window, renderer, 2);
                }
                else if (retFSM.catSel == catState::CHEB2)
                {
                    retFSM.catRet = drawCategory(window, renderer, 3);
                }
                else if(retFSM.catSel == catState::BESL)
                {
                    retFSM.catRet = drawCategory(window, renderer, 4);
                }
                else if(retFSM.catSel == catState::ELIP)
                {
                    retFSM.catRet = drawCategory(window, renderer, 5);
                }
                retFSM.currScreen = screenState::CATEGORY;
            break;

            case screenState::CATEGORY:
                // Filter Type Static (Butt, Cheb1, Cheb2, Besl, Elip)
                switch (retFSM.catSel)
                {
                    case catState::BUTT:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::CHEB1;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                    break;

                    case catState::CHEB1:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::BUTT;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::CHEB2;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                    break;

                    case catState::CHEB2:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::CHEB1;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::BESL;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                    break;

                    case catState::BESL:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::CHEB2;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                        else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::ELIP;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                    break;

                    case catState::ELIP:
                        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                        {
                            retFSM.catSel = catState::BESL;
                            retFSM.currScreen = screenState::CATEGORY_REND;
                        }
                    break;
                    
                    default:
                    break;
                }
                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.catRet.submit.x &&
                    event->button.x <= (retFSM.catRet.submit.x + retFSM.catRet.submit.w) &&
                    event->button.y >= retFSM.catRet.submit.y &&
                    event->button.y <= (retFSM.catRet.submit.y + retFSM.catRet.submit.h))
                {
                    // Submit button clicked
                    deallocateTypeResources(retFSM.catRet);
                    shared.category = retFSM.catSel;
                    retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                }

                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.catRet.back.x &&
                    event->button.x <= (retFSM.catRet.back.x + retFSM.catRet.back.w) &&
                    event->button.y >= retFSM.catRet.back.y &&
                    event->button.y <= (retFSM.catRet.back.y + retFSM.catRet.back.h))
                {
                    // Back button clicked
                    deallocateTypeResources(retFSM.catRet);
                    shared.category = retFSM.catSel;
                    retFSM.currScreen = screenState::TOPOLOGY_REND;
                }
            break;

            case screenState::CHARACTERISTICS_REND:
                // Filter Characteristics Render(Gain, Cut Off)
                if (retFSM.catSel == catState::BUTT)
                {
                    if(retFSM.charSel == charState::STOPBAND || retFSM.charSel == charState::MAXOSC || retFSM.charSel == charState::STOPBANDATT)
                    {
                        retFSM.charSel = charState::GAIN;
                    }
                    retFSM.charRet = drawCharacteristics(window, renderer, retFSM.charRet.ui, retFSM.catSel, retFSM.charSel, {"Gain:", "Cut-Off Frequency(Hz):"}, retFSM.charSubmitClicked);
                }
                else if (retFSM.catSel == catState::CHEB1)
                {
                    retFSM.charRet = drawCharacteristics(window, renderer, retFSM.charRet.ui, retFSM.catSel, retFSM.charSel, {"Gain:", "Passband Cut-Off Frequency(Hz):", "Stopband Cut-Off Frequency(Hz):", "Max Passband Oscillation(dB):", "Stopband Attenuation:"}, retFSM.charSubmitClicked);
                }
                else if (retFSM.catSel == catState::CHEB2)
                {
                    retFSM.charRet = drawCharacteristics(window, renderer, retFSM.charRet.ui, retFSM.catSel, retFSM.charSel, {"Gain:", "Passband Cut-Off Frequency(Hz):", "Stopband Cut-Off Frequency(Hz):", "Max Stopband Oscillation(dB):", "Stopband Attenuation:"}, retFSM.charSubmitClicked);
                }
                else if(retFSM.catSel == catState::BESL)
                {
                    if(retFSM.charSel == charState::STOPBAND || retFSM.charSel == charState::MAXOSC || retFSM.charSel == charState::STOPBANDATT)
                    {
                        retFSM.charSel = charState::GAIN;
                    }
                    retFSM.charRet = drawCharacteristics(window, renderer, retFSM.charRet.ui, retFSM.catSel, retFSM.charSel, {"Gain:", "Cut-Off Frequency(Hz):"}, retFSM.charSubmitClicked);
                }
                else if(retFSM.catSel == catState::ELIP)
                {
                    retFSM.charRet = drawCharacteristics(window, renderer, retFSM.charRet.ui, retFSM.catSel, retFSM.charSel, {"???"}, retFSM.charSubmitClicked);
                }
                retFSM.currScreen = screenState::CHARACTERISTICS;
            break;

            case screenState::CHARACTERISTICS:
                // Filter Characteristics Static(Gain, Cut Off)
                switch (retFSM.charSel)
                {
                case charState::GAIN:
                    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::CUTOFF;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_TEXTINPUT || event->type == SDL_KEYDOWN)
                    {
                        // Get the name of the pressed key
                        const char* keyName = SDL_GetKeyName(event->key.keysym.sym);

                        // Check if the key name corresponds to a digit from the numpad
                        if (strncmp(keyName, "Keypad ", 7) == 0 && isdigit(keyName[7]))
                        {
                            char digit = keyName[7];
                            // Handle text input events
                            if (retFSM.charRet.ui.gain == "0")
                            {
                                retFSM.charRet.ui.gain = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.gain += digit;
                            }
                        }
                        else if (isdigit(keyName[0]))
                        {
                            char digit = static_cast<char>(event->key.keysym.sym);
                            // Handle text input events
                            if (retFSM.charRet.ui.gain == "0")
                            {
                                retFSM.charRet.ui.gain = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.gain += digit;
                            }
                        }
                        else if (event->key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (retFSM.charRet.ui.gain.length() == 1)
                            {
                                retFSM.charRet.ui.gain = "0";
                            }
                            else
                            {
                                retFSM.charRet.ui.gain.pop_back();
                            }
                        }
                        retFSM.charChangeMade = true;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    break;

                case charState::CUTOFF:
                    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::GAIN;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED && (retFSM.catSel == catState::CHEB1 || retFSM.catSel == catState::CHEB2 || retFSM.catSel == catState::ELIP))
                    {
                        retFSM.charSel = charState::STOPBAND;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_TEXTINPUT || event->type == SDL_KEYDOWN)
                    {
                        // Get the name of the pressed key
                        const char* keyName = SDL_GetKeyName(event->key.keysym.sym);

                        // Check if the key name corresponds to a digit from the numpad
                        if (strncmp(keyName, "Keypad ", 7) == 0 && isdigit(keyName[7]))
                        {
                            char digit = keyName[7];
                            // Handle text input events
                            if (retFSM.charRet.ui.cutoffFrequency == "0")
                            {
                                retFSM.charRet.ui.cutoffFrequency = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.cutoffFrequency += digit;
                            }
                        }
                        else if (isdigit(keyName[0]))
                        {
                            char digit = static_cast<char>(event->key.keysym.sym);
                            // Handle text input events
                            if (retFSM.charRet.ui.cutoffFrequency == "0")
                            {
                                retFSM.charRet.ui.cutoffFrequency = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.cutoffFrequency += digit;
                            }
                        }
                        else if (event->key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (retFSM.charRet.ui.cutoffFrequency.length() == 1)
                            {
                                retFSM.charRet.ui.cutoffFrequency = "0";
                            }
                            else
                            {
                                retFSM.charRet.ui.cutoffFrequency.pop_back();
                            }
                        }
                        retFSM.charChangeMade = true;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    break;
                case charState::STOPBAND:
                    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::CUTOFF;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::MAXOSC;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_TEXTINPUT || event->type == SDL_KEYDOWN)
                    {
                        // Get the name of the pressed key
                        const char* keyName = SDL_GetKeyName(event->key.keysym.sym);

                        // Check if the key name corresponds to a digit from the numpad
                        if (strncmp(keyName, "Keypad ", 7) == 0 && isdigit(keyName[7]))
                        {
                            char digit = keyName[7];
                            // Handle text input events
                            if (retFSM.charRet.ui.stopbandFrequency == "0")
                            {
                                retFSM.charRet.ui.stopbandFrequency = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.stopbandFrequency += digit;
                            }
                        }
                        else if (isdigit(keyName[0]))
                        {
                            char digit = static_cast<char>(event->key.keysym.sym);
                            // Handle text input events
                            if (retFSM.charRet.ui.stopbandFrequency == "0")
                            {
                                retFSM.charRet.ui.stopbandFrequency = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.stopbandFrequency += digit;
                            }
                        }
                        else if (event->key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (retFSM.charRet.ui.stopbandFrequency.length() == 1)
                            {
                                retFSM.charRet.ui.stopbandFrequency = "0";
                            }
                            else
                            {
                                retFSM.charRet.ui.stopbandFrequency.pop_back();
                            }
                        }
                        retFSM.charChangeMade = true;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    break;
                case charState::MAXOSC:
                    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::STOPBAND;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_DOWN && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::STOPBANDATT;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_TEXTINPUT || event->type == SDL_KEYDOWN)
                    {
                        // Get the name of the pressed key
                        const char* keyName = SDL_GetKeyName(event->key.keysym.sym);

                        // Check if the key name corresponds to a digit from the numpad
                        if (strncmp(keyName, "Keypad ", 7) == 0 && isdigit(keyName[7]))
                        {
                            char digit = keyName[7];
                            // Handle text input events
                            if (retFSM.charRet.ui.maxOscillation == "0")
                            {
                                retFSM.charRet.ui.maxOscillation = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.maxOscillation += digit;
                            }
                        }
                        else if (isdigit(keyName[0]))
                        {
                            char digit = static_cast<char>(event->key.keysym.sym);
                            // Handle text input events
                            if (retFSM.charRet.ui.maxOscillation == "0")
                            {
                                retFSM.charRet.ui.maxOscillation = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.maxOscillation += digit;
                            }
                        }
                        else if (event->key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (retFSM.charRet.ui.maxOscillation.length() == 1)
                            {
                                retFSM.charRet.ui.maxOscillation = "0";
                            }
                            else
                            {
                                retFSM.charRet.ui.maxOscillation.pop_back();
                            }
                        }
                        retFSM.charChangeMade = true;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    break;
                case charState::STOPBANDATT:
                    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_UP && event->key.state == SDL_PRESSED)
                    {
                        retFSM.charSel = charState::MAXOSC;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    else if (event->type == SDL_TEXTINPUT || event->type == SDL_KEYDOWN)
                    {
                        // Get the name of the pressed key
                        const char* keyName = SDL_GetKeyName(event->key.keysym.sym);

                        // Check if the key name corresponds to a digit from the numpad
                        if (strncmp(keyName, "Keypad ", 7) == 0 && isdigit(keyName[7]))
                        {
                            char digit = keyName[7];
                            // Handle text input events
                            if (retFSM.charRet.ui.stopbandAttentuation == "0")
                            {
                                retFSM.charRet.ui.stopbandAttentuation = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.stopbandAttentuation += digit;
                            }
                        }
                        else if (isdigit(keyName[0]))
                        {
                            char digit = static_cast<char>(event->key.keysym.sym);
                            // Handle text input events
                            if (retFSM.charRet.ui.stopbandAttentuation == "0")
                            {
                                retFSM.charRet.ui.stopbandAttentuation = digit;
                            }
                            else
                            {
                                retFSM.charRet.ui.stopbandAttentuation += digit;
                            }
                        }
                        else if (event->key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (retFSM.charRet.ui.stopbandAttentuation.length() == 1)
                            {
                                retFSM.charRet.ui.stopbandAttentuation = "0";
                            }
                            else
                            {
                                retFSM.charRet.ui.stopbandAttentuation.pop_back();
                            }
                        }
                        retFSM.charChangeMade = true;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                    break;

                default:

                    break;
                }
                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.charRet.submit.x &&
                    event->button.x <= (retFSM.charRet.submit.x + retFSM.charRet.submit.w) &&
                    event->button.y >= retFSM.charRet.submit.y &&
                    event->button.y <= (retFSM.charRet.submit.y + retFSM.charRet.submit.h))
                {
                    // Submit button clicked
                    shared.cutOff = std::stoi(retFSM.charRet.ui.cutoffFrequency);
                    shared.gain = std::stoi(retFSM.charRet.ui.gain);
                    if(retFSM.catSel == catState::CHEB1 || retFSM.catSel == catState::CHEB2 || retFSM.catSel == catState::ELIP)
                    {
                        shared.stopFreq = std::stoi(retFSM.charRet.ui.stopbandFrequency);
	                    shared.maxOsc = std::stoi(retFSM.charRet.ui.maxOscillation);
	                    shared.stopAtt = std::stoi(retFSM.charRet.ui.stopbandAttentuation);
                        orderCheby1Calc();
                        if(!retFSM.charSubmitClicked)
                        {
                            retFSM.charSubmitClicked = true;
                        }
                        else if(retFSM.charSubmitClicked && retFSM.charRet.confirmShown && !retFSM.charChangeMade)
                        {
                            deallocateCharResources(retFSM.charRet, retFSM.catSel);
                            retFSM.currScreen = screenState::GENERATING_REND;
                            break;
                        }
                        retFSM.charChangeMade = false;
                        retFSM.currScreen = screenState::CHARACTERISTICS_REND;
                    }
                }

                if (event->type == SDL_MOUSEBUTTONUP &&
                    event->button.x >= retFSM.charRet.back.x &&
                    event->button.x <= (retFSM.charRet.back.x + retFSM.charRet.back.w) &&
                    event->button.y >= retFSM.charRet.back.y &&
                    event->button.y <= (retFSM.charRet.back.y + retFSM.charRet.back.h))
                {
                    // Back button clicked
                    shared.cutOff = std::stoi(retFSM.charRet.ui.cutoffFrequency);
                    shared.gain = std::stoi(retFSM.charRet.ui.gain);
                    retFSM.charSubmitClicked = false;
                    deallocateCharResources(retFSM.charRet, retFSM.catSel);
                    if(retFSM.typeSel == typeState::STATE)
                    {
                        retFSM.currScreen = screenState::TYPE_REND;
                    }
                    else
                    {
                        retFSM.currScreen = screenState::CATEGORY_REND;
                    }
                }
            break;

            case screenState::GENERATING_REND:
                // Renders the generating screen and shows transfer function
                shared.transfer = findTransferFunct();
                drawGenerating(window, renderer, shared.transfer);
                retFSM.currScreen = screenState::GENERATING;
            break;

            case screenState::GENERATING:
                // Static generating screen while finds graph and filter

            break;

            case screenState::RESULT:
                // Results
            break;

            default:

            break;
        }
        return retFSM;
    }

}


dealocTitRes drawTitle(SDL_Window* window, SDL_Renderer* renderer)
{
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("./Libraries used/Roboto/Roboto-Regular.ttf", 30);
    if (!font)
    {
    printf("Error loading font: %s\n", TTF_GetError());
    exit(1);
    }
    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Filter Design Tool", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create instructions text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto/Roboto-Regular.ttf", 26);
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


dealocTypRes drawType(SDL_Window* window, SDL_Renderer* renderer, int selectedType)
{
    dealocTypRes resRet;
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 30);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Filter Type", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create instructions text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 21);
    SDL_Surface* instructionsSurface = TTF_RenderText_Solid(font, "Use Arrow Keys to Make Selection", textColor);
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
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Draw instructions texture below the title
    SDL_Rect instructionsRect = { (winWidth - instructionsWidth) / 2, titleRect.y + titleRect.h + 24, instructionsWidth, instructionsHeight };
    SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

    // Create list of filter types
    std::vector<std::string> filterTypes = { "Low Pass", "High Pass", "Band Pass", "Notch", "State Variable" };

    // Get filter type texture dimensions
    int maxFilterWidth = 0, filterHeight = 0;
    std::vector<int> filterWidths;
    std::vector<SDL_Texture*> filterTextures;
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 26);
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

    // Calculate the maximum width of filter types
    for (const auto& width : filterWidths) {
        if (width > maxFilterWidth) {
            maxFilterWidth = width;
        }
    }

    // Calculate the starting X position
    int startX = (winWidth - maxFilterWidth) / 2;

    // Draw filter type textures in a list
    int startY = instructionsRect.y + instructionsRect.h + 50;
    for (size_t i = 0; i < filterTypes.size(); i++)
    {
        int filterWidth = filterWidths[i];
        int xPos = startX;
        SDL_Rect filterRect = { xPos, startY + i * filterHeight, filterWidth, filterHeight };
        SDL_RenderCopy(renderer, filterTextures[i], nullptr, &filterRect);
    }
    // Highlight the selected filter type
    SDL_Color color = { 0, 0, 0, 127 };
    int xPos = (winWidth - maxFilterWidth - 10) / 2;
    int yPos = startY + selectedType * filterHeight - 14;
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

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Create back button texture
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, "<- Back", textColor);
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);

    // Get button texture dimensions
    int backWidth, backHeight;
    SDL_QueryTexture(backTexture, NULL, NULL, &backWidth, &backHeight);

    // Draw button texture at bottom center of screen
    SDL_Rect backRect = { 30, 30, backWidth, backHeight };
    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backRect);

    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Assign the buttonRect values to resRet.submit
    resRet.submit = buttonRect;
    resRet.back = backRect;

    // Assign resources that must be deallocated
    resRet.buttonSurface = buttonSurface;
    resRet.buttonTexture = buttonTexture;
    resRet.backSurface = backSurface;
    resRet.backTexture = backTexture;
    resRet.filterTextures = filterTextures;
    resRet.font = font;
    resRet.titleSurface = titleSurface;
    resRet.titleTexture = titleTexture;
    resRet.instructionsSurface = instructionsSurface;
    resRet.instructionsTexture = instructionsTexture;

    // Present the renderer
    SDL_RenderPresent(renderer);

    return resRet;
}


void deallocateTypeResources(dealocTypRes typResRet)
{
    for (auto& texture : typResRet.filterTextures)
    {
        SDL_DestroyTexture(texture);
    }
    TTF_CloseFont(typResRet.font);
    SDL_FreeSurface(typResRet.buttonSurface);
    SDL_DestroyTexture(typResRet.buttonTexture);
    SDL_FreeSurface(typResRet.backSurface);
    SDL_DestroyTexture(typResRet.backTexture);
    SDL_FreeSurface(typResRet.titleSurface);
    SDL_DestroyTexture(typResRet.titleTexture);
    SDL_FreeSurface(typResRet.instructionsSurface);
    SDL_DestroyTexture(typResRet.instructionsTexture);
}

dealocTypRes drawTopology(SDL_Window* window, SDL_Renderer* renderer, std::vector<std::string> filterTypes, int selectedTopology)
{
    dealocTypRes resRet;
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 30);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Filter Topology", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create instructions text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 21);
    SDL_Surface* instructionsSurface = TTF_RenderText_Solid(font, "Use Arrow Keys to Make Selection", textColor);
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
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Draw instructions texture below the title
    SDL_Rect instructionsRect = { (winWidth - instructionsWidth) / 2, titleRect.y + titleRect.h + 24, instructionsWidth, instructionsHeight };
    SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

    // Get filter type texture dimensions
    int maxFilterWidth = 0, filterHeight = 0;
    std::vector<int> filterWidths;
    std::vector<SDL_Texture*> filterTextures;
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 26);
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

    // Calculate the maximum width of filter types
    for (const auto& width : filterWidths) {
        if (width > maxFilterWidth) {
            maxFilterWidth = width;
        }
    }

    // Calculate the starting X position
    int startX = (winWidth - maxFilterWidth) / 2;

    // Draw filter type textures in a list
    int startY = instructionsRect.y + instructionsRect.h + 50;
    for (size_t i = 0; i < filterTypes.size(); i++)
    {
        int filterWidth = filterWidths[i];
        int xPos = startX;
        SDL_Rect filterRect = { xPos, startY + i * filterHeight, filterWidth, filterHeight };
        SDL_RenderCopy(renderer, filterTextures[i], nullptr, &filterRect);
    }
    // Circle the selected filter type
    SDL_Color color = { 0, 0, 0, 127 };
    int xPos = (winWidth - maxFilterWidth - 10) / 2;
    int yPos = startY + selectedTopology * filterHeight - 14;
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

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Create back button texture
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, "<- Back", textColor);
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);

    // Get button texture dimensions
    int backWidth, backHeight;
    SDL_QueryTexture(backTexture, NULL, NULL, &backWidth, &backHeight);

    // Draw button texture at bottom center of screen
    SDL_Rect backRect = { 30, 30, backWidth, backHeight };
    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backRect);

    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Assign the buttonRect values to resRet.submit
    resRet.submit = buttonRect;
    resRet.back = backRect;

    // Assign resources that must be deallocated
    resRet.buttonSurface = buttonSurface;
    resRet.buttonTexture = buttonTexture;
    resRet.backSurface = backSurface;
    resRet.backTexture = backTexture;
    resRet.filterTextures = filterTextures;
    resRet.font = font;
    resRet.titleSurface = titleSurface;
    resRet.titleTexture = titleTexture;
    resRet.instructionsSurface = instructionsSurface;
    resRet.instructionsTexture = instructionsTexture;

    // Present the renderer
    SDL_RenderPresent(renderer);

    return resRet;
}

dealocTypRes drawCategory(SDL_Window* window, SDL_Renderer* renderer, int selectedType)
{
    dealocTypRes resRet;
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 30);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Filter Category", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create instructions text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 21);
    SDL_Surface* instructionsSurface = TTF_RenderText_Solid(font, "Use Arrow Keys to Make Selection", textColor);
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
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Draw instructions texture below the title
    SDL_Rect instructionsRect = { (winWidth - instructionsWidth) / 2, titleRect.y + titleRect.h + 24, instructionsWidth, instructionsHeight };
    SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

    // Create list of filter types
    std::vector<std::string> filterTypes = { "Butterworth", "Chebyshev I", "Chebyshev II", "Bessel", "Elliptic" };

    // Get filter type texture dimensions
    int maxFilterWidth = 0, filterHeight = 0;
    std::vector<int> filterWidths;
    std::vector<SDL_Texture*> filterTextures;
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 26);
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

    // Calculate the starting X position
    int startX = (winWidth - maxFilterWidth) / 2;

    // Draw filter type textures in a list
    int startY = instructionsRect.y + instructionsRect.h + 50;
    for (size_t i = 0; i < filterTypes.size(); i++)
    {
        int filterWidth = filterWidths[i];
        int xPos = startX;
        SDL_Rect filterRect = { xPos, startY + i * filterHeight, filterWidth, filterHeight };
        SDL_RenderCopy(renderer, filterTextures[i], nullptr, &filterRect);
    }
    // Highlight the selected filter type
    SDL_Color color = { 0, 0, 0, 127 };
    int xPos = (winWidth - maxFilterWidth - 10) / 2;
    int yPos = startY + selectedType * filterHeight - 14;
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

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Create back button texture
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, "<- Back", textColor);
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);

    // Get button texture dimensions
    int backWidth, backHeight;
    SDL_QueryTexture(backTexture, NULL, NULL, &backWidth, &backHeight);

    // Draw button texture at bottom center of screen
    SDL_Rect backRect = { 30, 30, backWidth, backHeight };
    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backRect);

    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Assign the buttonRect values to resRet.submit
    resRet.submit = buttonRect;
    resRet.back = backRect;

    // Assign resources that must be deallocated
    resRet.buttonSurface = buttonSurface;
    resRet.buttonTexture = buttonTexture;
    resRet.backSurface = backSurface;
    resRet.backTexture = backTexture;
    resRet.filterTextures = filterTextures;
    resRet.font = font;
    resRet.titleSurface = titleSurface;
    resRet.titleTexture = titleTexture;
    resRet.instructionsSurface = instructionsSurface;
    resRet.instructionsTexture = instructionsTexture;

    // Present the renderer
    SDL_RenderPresent(renderer);

    return resRet;
}

dealocCharRes drawCharacteristics(SDL_Window* window, SDL_Renderer* renderer, const CharacteristicInputs& inputs, catState catSel, charState charSel, std::vector<std::string> filterCharacteristics, bool submitClicked)
{
    dealocCharRes resRet;
    resRet.confirmShown = false;
    // Load font and set font size
    SDL_Color textColor = { 0, 0, 0 };

    // Create title text surface and texture
    TTF_Font* font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 30);
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Filter Characteristics", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create instructions text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 21);
    SDL_Surface* instructionsSurface = TTF_RenderText_Solid(font, "Select Textbox and Enter Values", textColor);
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
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Draw instructions texture below the title
    SDL_Rect instructionsRect = { (winWidth - instructionsWidth) / 2, titleRect.y + titleRect.h + 24, instructionsWidth, instructionsHeight };
    SDL_RenderCopy(renderer, instructionsTexture, NULL, &instructionsRect);

    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 24);

    int maxFilterWidth = 0, filterHeight = 0;
    std::vector<int> filterWidths;
    std::vector<SDL_Texture*> characteristicTextures;
    std::vector<SDL_Surface*> characteristicSurfaces;
    for (const auto& characteristic : filterCharacteristics)
    {
        SDL_Surface* characteristicSurface = TTF_RenderText_Solid(font, characteristic.c_str(), textColor);
        SDL_Texture* characteristicTexture = SDL_CreateTextureFromSurface(renderer, characteristicSurface);
        characteristicTextures.push_back(characteristicTexture);
        characteristicSurfaces.push_back(characteristicSurface);
        int filterWidth;
        SDL_QueryTexture(characteristicTexture, NULL, NULL, &filterWidth, &filterHeight);
        filterWidths.push_back(filterWidth);
        if (filterWidth > maxFilterWidth) {
            maxFilterWidth = filterWidth;
        }
    }
    // Universal Noting that cutoff frequency is passband cut off frequency
    // Create gain value text surface and texture
    SDL_Surface* gainValueSurface = TTF_RenderText_Solid(font, inputs.gain.c_str(), textColor);
    SDL_Texture* gainValueTexture = SDL_CreateTextureFromSurface(renderer, gainValueSurface);

    // Create cutoff frequency value text surface and texture
    SDL_Surface* cutoffValueSurface = TTF_RenderText_Solid(font, inputs.cutoffFrequency.c_str(), textColor);
    SDL_Texture* cutoffValueTexture = SDL_CreateTextureFromSurface(renderer, cutoffValueSurface);

    // Get gain texture dimensions
    int gainWidth, gainHeight;
    SDL_QueryTexture(gainValueTexture, NULL, NULL, &gainWidth, &gainHeight);

    // Get cut off texture dimensions
    int cutoffWidth, cutoffHeight;
    SDL_QueryTexture(cutoffValueTexture, NULL, NULL, &cutoffWidth, &cutoffHeight);

    // Render the gain label texture
    SDL_Rect gainLabelRect = { (winWidth - gainWidth) / 3, instructionsRect.y + instructionsRect.h + 48, characteristicSurfaces[0]->w, characteristicSurfaces[0]->h };
    SDL_RenderCopy(renderer, characteristicTextures[0], nullptr, &gainLabelRect);

    // Render the cutoff frequency label texture
    SDL_Rect cutoffLabelRect = { (winWidth - cutoffWidth) / 3,  gainLabelRect.y + gainLabelRect.h + 24, characteristicSurfaces[1]->w, characteristicSurfaces[1]->h };
    SDL_RenderCopy(renderer, characteristicTextures[1], nullptr, &cutoffLabelRect);

    // Render the gain value text box texture
    SDL_Rect gainRect = { gainLabelRect.x + gainLabelRect.w + 10, instructionsRect.y + instructionsRect.h + 52, gainValueSurface->w, gainValueSurface->h };

    // Render the cutoff frequency value text box texture
    SDL_Rect cutoffRect = { cutoffLabelRect.x + cutoffLabelRect.w + 10, gainLabelRect.y + gainLabelRect.h + 28 , cutoffValueSurface->w, cutoffValueSurface->h };

    // Darken Selection
    if (charSel == charState::GAIN)
    {
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Set the color for filling the rectangle
        SDL_RenderFillRect(renderer, &gainRect); // Fill the rectangle on the renderer
    }
    else if (charSel == charState::CUTOFF)
    {
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Set the color for filling the rectangle
        SDL_RenderFillRect(renderer, &cutoffRect); // Fill the rectangle on the renderer
    }

    // Draw box around the gain value
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &gainRect);
    SDL_RenderCopy(renderer, gainValueTexture, nullptr, &gainRect);

    // Draw box around cutoff frequency value
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &cutoffRect);
    SDL_RenderCopy(renderer, cutoffValueTexture, nullptr, &cutoffRect);

    // Additional for: CHEB1, CHEB2, ELIP
    if(catSel == catState::CHEB1 || catSel == catState::CHEB2)
    {
        //"Stopband Cut-Off Frequency(Hz):", "Max Passband Oscillation:", "Stopband Attenuation:"
        SDL_Surface* stopbandFrequencyValueSurface = TTF_RenderText_Solid(font, inputs.stopbandFrequency.c_str(), textColor);
        SDL_Texture* stopbandFrequencyValueTexture = SDL_CreateTextureFromSurface(renderer, stopbandFrequencyValueSurface);

        SDL_Surface* maxOscillationValueSurface = TTF_RenderText_Solid(font, inputs.maxOscillation.c_str(), textColor);
        SDL_Texture* maxOscillationValueTexture = SDL_CreateTextureFromSurface(renderer, maxOscillationValueSurface);
        
        SDL_Surface* stopbandAttentuationValueSurface = TTF_RenderText_Solid(font, inputs.stopbandAttentuation.c_str(), textColor);
        SDL_Texture* stopbandAttentuationValueTexture = SDL_CreateTextureFromSurface(renderer, stopbandAttentuationValueSurface);

        //For placement calculations
        int stopbandFrequencyWidth, stopbandFrequencyHeight;
        SDL_QueryTexture(stopbandFrequencyValueTexture, NULL, NULL, &stopbandFrequencyWidth, &stopbandFrequencyHeight);

        int maxOscillationWidth, maxOscillationHeight;
        SDL_QueryTexture(maxOscillationValueTexture, NULL, NULL, &maxOscillationWidth, &maxOscillationHeight);

        int stopbandAttentuationWidth, stopbandAttentuationHeight;
        SDL_QueryTexture(stopbandAttentuationValueTexture, NULL, NULL, &stopbandAttentuationWidth, &stopbandAttentuationHeight);

        // Render the stopbandFrequency label texture
        SDL_Rect stopbandFrequencyLabelRect = { (winWidth - stopbandFrequencyWidth) / 3, cutoffLabelRect.y + cutoffLabelRect.h + 24, characteristicSurfaces[2]->w, characteristicSurfaces[2]->h };
        SDL_RenderCopy(renderer, characteristicTextures[2], nullptr, &stopbandFrequencyLabelRect);

        // Render the maxOscillation label texture
        SDL_Rect maxOscillationLabelRect = { (winWidth - maxOscillationWidth) / 3,  stopbandFrequencyLabelRect.y + stopbandFrequencyLabelRect.h + 24, characteristicSurfaces[3]->w, characteristicSurfaces[3]->h };
        SDL_RenderCopy(renderer, characteristicTextures[3], nullptr, &maxOscillationLabelRect);

        // Render the stopbandAttentuation label texture
        SDL_Rect stopbandAttentuationLabelRect = { (winWidth - stopbandAttentuationWidth) / 3,  maxOscillationLabelRect.y + maxOscillationLabelRect.h + 24, characteristicSurfaces[4]->w, characteristicSurfaces[4]->h };
        SDL_RenderCopy(renderer, characteristicTextures[4], nullptr, &stopbandAttentuationLabelRect);

        // Render the stopbandFrequency value text box texture
        SDL_Rect stopbandFrequencyRect = { stopbandFrequencyLabelRect.x + stopbandFrequencyLabelRect.w + 10, cutoffLabelRect.y + cutoffLabelRect.h + 28, stopbandFrequencyValueSurface->w, stopbandFrequencyValueSurface->h };

        // Render the maxOscillation value text box texture
        SDL_Rect maxOscillationRect = { maxOscillationLabelRect.x + maxOscillationLabelRect.w + 10, stopbandFrequencyLabelRect.y + stopbandFrequencyLabelRect.h + 28, maxOscillationValueSurface->w, maxOscillationValueSurface->h };

        // Render the stopbandAttentuation value text box texture
        SDL_Rect stopbandAttentuationRect = { stopbandAttentuationLabelRect.x + stopbandAttentuationLabelRect.w + 10, maxOscillationLabelRect.y + maxOscillationLabelRect.h + 28 , stopbandAttentuationValueSurface->w, stopbandAttentuationValueSurface->h };

        // Darken Selection
        if (charSel == charState::STOPBAND)
        {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Set the color for filling the rectangle
            SDL_RenderFillRect(renderer, &stopbandFrequencyRect); // Fill the rectangle on the renderer
        }
        else if (charSel == charState::MAXOSC)
        {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Set the color for filling the rectangle
            SDL_RenderFillRect(renderer, &maxOscillationRect); // Fill the rectangle on the renderer
        }
        else if (charSel == charState::STOPBANDATT)
        {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Set the color for filling the rectangle
            SDL_RenderFillRect(renderer, &stopbandAttentuationRect); // Fill the rectangle on the renderer
        }

        // Draw box around the stopbandFrequency value
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &stopbandFrequencyRect);
        SDL_RenderCopy(renderer, stopbandFrequencyValueTexture, nullptr, &stopbandFrequencyRect);

        // Draw box around maxOscillation value
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &maxOscillationRect);
        SDL_RenderCopy(renderer, maxOscillationValueTexture, nullptr, &maxOscillationRect);

        // Draw box around stopbandAttentuation value
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &stopbandAttentuationRect);
        SDL_RenderCopy(renderer, stopbandAttentuationValueTexture, nullptr, &stopbandAttentuationRect);

        resRet.stopbandFrequencyValueSurface = stopbandFrequencyValueSurface;
        resRet.stopbandFrequencyValueTexture = stopbandFrequencyValueTexture;
        resRet.maxOscillationValueSurface = maxOscillationValueSurface;
        resRet.maxOscillationValueTexture = maxOscillationValueTexture;
        resRet.stopbandAttentuationValueSurface = stopbandAttentuationValueSurface;
        resRet.stopbandAttentuationValueTexture = stopbandAttentuationValueTexture;
    }
    else if(catSel == catState::ELIP)
    {
        //?
        //SDL_Surface* gainValueSurface = TTF_RenderText_Solid(font, inputs.gain.c_str(), textColor);
        //SDL_Texture* gainValueTexture = SDL_CreateTextureFromSurface(renderer, gainValueSurface);
    }

    textColor = { 0, 0, 0 };
    // Create submit button texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 26);
    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Submit", textColor);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);

    // Get button texture dimensions
    int buttonWidth, buttonHeight;
    SDL_QueryTexture(buttonTexture, NULL, NULL, &buttonWidth, &buttonHeight);

    // Draw button texture at bottom center of screen
    SDL_Rect buttonRect = { (winWidth - buttonWidth) / 2, winHeight - buttonHeight - 50, buttonWidth, buttonHeight };
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);

    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonRect);

    // Create order message
    if (submitClicked)
    {
        font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 21);
        std::string explanation;
        std::string orderMessage = "The order is: " + std::to_string(shared.order);
        if(shared.order > 2.0)
        {
            textColor = { 255, 0, 0 };
            explanation = "Change the parameters such that the order is less than 2.0";
        }
        else
        {
            textColor = { 0, 255, 0 };
            resRet.confirmShown = true;
            explanation = "Submit again to confirm your input";
        }
        SDL_Surface* orderSurface = TTF_RenderText_Solid(font, orderMessage.c_str(), textColor);
        SDL_Texture* orderTexture = SDL_CreateTextureFromSurface(renderer, orderSurface);
        SDL_Surface* explanationSurface = TTF_RenderText_Solid(font, explanation.c_str(), textColor);
        SDL_Texture* explanationTexture = SDL_CreateTextureFromSurface(renderer, explanationSurface);

        int orderWidth, orderHeight;
        SDL_QueryTexture(orderTexture, NULL, NULL, &orderWidth, &orderHeight);
        int explanationWidth, explanationHeight;
        SDL_QueryTexture(explanationTexture, NULL, NULL, &explanationWidth, &explanationHeight);

        SDL_Rect explanationRect = { (winWidth - explanationWidth) / 2, buttonRect.y - (explanationHeight + 50), explanationWidth, explanationHeight };
        SDL_RenderCopy(renderer, explanationTexture, NULL, &explanationRect);
        SDL_Rect orderRect = { (winWidth - orderWidth) / 2, explanationRect.y + (orderHeight + 4), orderWidth, orderHeight };
        SDL_RenderCopy(renderer, orderTexture, NULL, &orderRect);
    }

    textColor = { 0, 0, 0 };
    // Create back button texture
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, "<- Back", textColor);
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);

    // Get button texture dimensions
    int backWidth, backHeight;
    SDL_QueryTexture(backTexture, NULL, NULL, &backWidth, &backHeight);

    // Draw button texture at bottom center of screen
    SDL_Rect backRect = { 30, 30, backWidth, backHeight };
    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Draw box around the button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &backRect);

    SDL_RenderCopy(renderer, backTexture, NULL, &backRect);

    // Assign the buttonRect values to resRet.submit
    resRet.submit = buttonRect;
    resRet.back = backRect;

    resRet.ui.cutoffFrequency = inputs.cutoffFrequency;
    resRet.ui.gain = inputs.gain;
    resRet.ui.stopbandFrequency = inputs.stopbandFrequency;
    resRet.ui.maxOscillation = inputs.maxOscillation;
    resRet.ui.stopbandAttentuation = inputs.stopbandAttentuation;

    // Assign resources that must be deallocated
    resRet.font = font;
    resRet.buttonSurface = buttonSurface;
    resRet.buttonTexture = buttonTexture;
    resRet.backSurface = backSurface;
    resRet.backTexture = backTexture;
    resRet.titleSurface = titleSurface;
    resRet.titleTexture = titleTexture;
    resRet.instructionsSurface = instructionsSurface;
    resRet.instructionsTexture = instructionsTexture;
    resRet.gainValueSurface = gainValueSurface;
    resRet.gainValueTexture = gainValueTexture;
    resRet.cutoffValueSurface = cutoffValueSurface;
    resRet.cutoffValueTexture = cutoffValueTexture;
    resRet.characteristicSurfaces = characteristicSurfaces;
    resRet.characteristicTextures = characteristicTextures;

    // Present the renderer
    SDL_RenderPresent(renderer);

    return resRet;

}

void deallocateCharResources(dealocCharRes charResRet, catState catSel)
{
    for (auto& surface : charResRet.characteristicSurfaces)
    {
        SDL_FreeSurface(surface);
    }
    for (auto& texture : charResRet.characteristicTextures)
    {
        SDL_DestroyTexture(texture);
    }
    TTF_CloseFont(charResRet.font);
    SDL_FreeSurface(charResRet.titleSurface);
    SDL_DestroyTexture(charResRet.titleTexture);
    SDL_FreeSurface(charResRet.instructionsSurface);
    SDL_DestroyTexture(charResRet.instructionsTexture);
    SDL_FreeSurface(charResRet.gainValueSurface);
    SDL_DestroyTexture(charResRet.gainValueTexture);
    SDL_FreeSurface(charResRet.cutoffValueSurface);
    SDL_DestroyTexture(charResRet.cutoffValueTexture);
    if(catSel == catState::CHEB1 || catSel == catState::CHEB2)
    {
        SDL_FreeSurface(charResRet.stopbandFrequencyValueSurface);
        SDL_DestroyTexture(charResRet.stopbandFrequencyValueTexture);
        SDL_FreeSurface(charResRet.maxOscillationValueSurface);
        SDL_DestroyTexture(charResRet.maxOscillationValueTexture);
        SDL_FreeSurface(charResRet.stopbandAttentuationValueSurface);
        SDL_DestroyTexture(charResRet.stopbandAttentuationValueTexture);
    }
}

void drawGenerating(SDL_Window* window, SDL_Renderer* renderer, transferFunct transfer)
{
    dealocTypRes resRet;
    // Load font and set font size
    TTF_Font* font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 30);

    // Create title text surface and texture
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Generating...", textColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Create numerator text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 21);
    SDL_Surface* numeratorSurface = TTF_RenderText_Solid(font, transfer.numerator.c_str(), textColor);
    SDL_Texture* numeratorTexture = SDL_CreateTextureFromSurface(renderer, numeratorSurface);
    // Create denominator text surface and texture
    SDL_Surface* denominatorSurface = TTF_RenderText_Solid(font, transfer.denominator.c_str(), textColor);
    SDL_Texture* denominatorTexture = SDL_CreateTextureFromSurface(renderer, denominatorSurface);

    // Create H(s) text surface and texture
    font = TTF_OpenFont("./Libraries used/Roboto_Mono/RobotoMono-VariableFont_wght.ttf", 24);
    SDL_Surface* hsSurface = TTF_RenderText_Solid(font, "H(s) = ", textColor);
    SDL_Texture* hsTexture = SDL_CreateTextureFromSurface(renderer, hsSurface);

    // Get window dimensions
    int winWidth, winHeight;
    SDL_GetWindowSize(window, &winWidth, &winHeight);

    // Get title texture dimensions
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    // Get numerator texture dimensions
    int numWidth, numHeight;
    SDL_QueryTexture(numeratorTexture, NULL, NULL, &numWidth, &numHeight);
    // Get denominator texture dimensions
    int denomWidth, denomHeight;
    SDL_QueryTexture(denominatorTexture, NULL, NULL, &denomWidth, &denomHeight);
    // Get H(s) texture dimensions
    int hsWidth, hsHeight;
    SDL_QueryTexture(hsTexture, NULL, NULL, &hsWidth, &hsHeight);

    int transferWidth = denominatorSurface->w;
    int transferHeight = denominatorSurface->h;

    // Calculate the total width of the transfer function and H(s) part
    int transferFunctionWidth = std::max(numWidth, denomWidth);
    int statementWidth = transferFunctionWidth + hsWidth + 12; // Add the width of H(s) and some padding

    // Calculate the total height of the transfer function and numerator/denominator
    int transferFunctionHeight = transferHeight; // Add some padding
    int statementHeight = transferFunctionHeight + std::max(numHeight, denomHeight);

    // Position Rectangles in a way that centers the entire statement under the title
    SDL_Rect titleRect = { (winWidth - titleWidth) / 2, (winHeight - titleHeight) / 2 - 150, titleWidth, titleHeight };
    SDL_Rect hsRect = { (winWidth - statementWidth) / 2, titleRect.y + titleRect.h + 80, hsWidth, hsHeight };
    SDL_Rect transferRect = { hsRect.x + hsWidth + 12, hsRect.y, transferFunctionWidth, transferFunctionHeight };
    SDL_Rect numeratorRect = { transferRect.x + (transferRect.w - numWidth) / 2, transferRect.y - numHeight + 12, numWidth, numHeight };
    SDL_Rect denominatorRect = { transferRect.x + (transferRect.w - denomWidth) / 2, transferRect.y + transferRect.h - 12, denomWidth, denomHeight };

    // Draw title texture in the center of the screen
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Draw numerator texture below the title
    SDL_RenderCopy(renderer, numeratorTexture, NULL, &numeratorRect);

    // Draw denominator texture below the title
    SDL_RenderCopy(renderer, denominatorTexture, NULL, &denominatorRect);

    // Draw a solid black line below the numerator
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, transferRect.x, transferRect.y + transferHeight / 2, transferRect.x + transferWidth, transferRect.y + transferHeight / 2);

    // Draw H(s) texture between denom and num
    SDL_RenderCopy(renderer, hsTexture, NULL, &hsRect);

    // Update the renderer
    SDL_RenderPresent(renderer);

}

void handleTextInputEvent(const SDL_Event& event, std::string& inputText)
{
    if (event.type == SDL_TEXTINPUT)
    {
        inputText += event.text.text;
    }
    else if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty())
        {
            inputText.pop_back();
        }
    }
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

