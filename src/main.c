#define SDL_MAIN_USE_CALLBACKS 1 // Use callback functions instead of main()

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// We will use this renderer to draw into this window every frame
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// This function runs once at startup
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("SDL3 Rectangles", 380, 380, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Enable VSync (1 = sync with every refresh)
    if (!SDL_SetRenderVSync(renderer, 1))
    {
        SDL_Log("Failed to enable VSync: %s", SDL_GetError());
    }

    return SDL_APP_CONTINUE; // Continue program execution!
}

// This function runs whenever a new event occurs (mouse input, key presses, etc.)
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS; // Terminate the program, reporting success to the OS
    }
    return SDL_APP_CONTINUE; // Continue program execution!
}

// This function runs once per frame and is the heart of the program
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 38, 43, 51, 255);

    // Clear the window with the draw color
    SDL_RenderClear(renderer);

    // Rectangle 1 (Red)
    SDL_FRect rect1 = { 50.0f, 150.0f, 100.0f, 50.0f };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect1);

    // Rectangle 2 (Green)
    SDL_FRect rect2 = { 200.0f, 150.0f, 100.0f, 50.0f };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect2);

    // Rectangle 3 (Blue)
    SDL_FRect rect3 = { 125.0f, 50.0f, 100.0f, 50.0f };
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &rect3);

    // Present the freshly cleared result to the screen
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE; // Continue program execution!
}

// This function runs once upon shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // SDL will clean up the window and renderer for us
}
