#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Rectangle dimensions
const int RECT_WIDTH = 50;
const int RECT_HEIGHT = 50;
const int COLLECTABLE_SIZE = 20; // Size of collectable squares

// Starts up SDL and creates window
bool init();

// Frees media and shuts down SDL
void close();

// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The window renderer
SDL_Renderer* gRenderer = nullptr;

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else
    {
        // Create window
        gWindow = SDL_CreateWindow("Pixel Dash", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr)
        {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            // Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == nullptr)
            {
                std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); // Set background color to black
            }
        }
    }

    return success;
}

void close()
{
    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char* args[])
{
    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(0)));

    // Start up SDL and create window
    if (!init())
    {
        std::cerr << "Failed to initialize!" << std::endl;
    }
    else
    {
        // Main loop flag
        bool quit = false;

        // Event handler
        SDL_Event e;

        // Rectangle position and velocity
        int rectX = (SCREEN_WIDTH - RECT_WIDTH) / 2;
        int rectY = (SCREEN_HEIGHT - RECT_HEIGHT) / 2;
        int velocityX = 0;
        int velocityY = 0;
        const int speed = 2; // Reduced speed

        // Enemy rectangle position and velocity
        int enemyX = SCREEN_WIDTH / 4;
        int enemyY = SCREEN_HEIGHT / 4;
        int enemyVelocityX = speed;
        int enemyVelocityY = speed;

        // Collectables
        std::vector<SDL_Rect> collectables = {
            { 100, 100, COLLECTABLE_SIZE, COLLECTABLE_SIZE },
            { 200, 150, COLLECTABLE_SIZE, COLLECTABLE_SIZE },
            { 300, 200, COLLECTABLE_SIZE, COLLECTABLE_SIZE },
            { 400, 250, COLLECTABLE_SIZE, COLLECTABLE_SIZE },
            { 500, 300, COLLECTABLE_SIZE, COLLECTABLE_SIZE }
        };

        // Player score
        int score = 0;

        // While application is running
        while (!quit)
        {
            // Handle events on queue
            while (SDL_PollEvent(&e) != 0)
            {
                // User requests quit
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                // Handle key press events
                else if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_UP:
                        velocityY = -speed;
                        break;
                    case SDLK_DOWN:
                        velocityY = speed;
                        break;
                    case SDLK_LEFT:
                        velocityX = -speed;
                        break;
                    case SDLK_RIGHT:
                        velocityX = speed;
                        break;
                    }
                }
                else if (e.type == SDL_KEYUP)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_UP:
                    case SDLK_DOWN:
                        velocityY = 0;
                        break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        velocityX = 0;
                        break;
                    }
                }
            }

            // Move the player rectangle
            rectX += velocityX;
            rectY += velocityY;

            // Move the enemy rectangle
            enemyX += enemyVelocityX;
            enemyY += enemyVelocityY;

            // Change enemy direction randomly
            if (std::rand() % 100 < 2) // 2% chance every frame to change direction
            {
                enemyVelocityX = (std::rand() % 3 - 1) * speed;
                enemyVelocityY = (std::rand() % 3 - 1) * speed;
            }

            // Keep enemy within screen bounds
            if (enemyX < 0 || enemyX + RECT_WIDTH > SCREEN_WIDTH)
            {
                enemyVelocityX = -enemyVelocityX;
            }
            if (enemyY < 0 || enemyY + RECT_HEIGHT > SCREEN_HEIGHT)
            {
                enemyVelocityY = -enemyVelocityY;
            }

            // Check collision with collectables
            SDL_Rect playerRect = { rectX, rectY, RECT_WIDTH, RECT_HEIGHT };
            for (auto it = collectables.begin(); it != collectables.end(); )
            {
                if (SDL_HasIntersection(&playerRect, &(*it)))
                {
                    // Collectable collected
                    it = collectables.erase(it);
                    score += 10; // Increase score by 10 for each collectable
                    std::cout << "Score: " << score << std::endl; // Display score on console
                }
                else
                {
                    ++it;
                }
            }

            // Check if all collectables are collected
            if (collectables.empty())
            {
                std::cout << "You Win! Final Score: " << score << std::endl;
                quit = true;
            }

            // Check collision with enemy
            SDL_Rect enemyRect = { enemyX, enemyY, RECT_WIDTH, RECT_HEIGHT };
            if (SDL_HasIntersection(&playerRect, &enemyRect))
            {
                // Player has collided with the enemy
                std::cout << "Player has died! Final Score: " << score << std::endl;
                quit = true;
            }

            // Clear screen
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF); // Set background color to black
            SDL_RenderClear(gRenderer);

            // Set render color to red (for the player rectangle)
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);

            // Render the player rectangle
            SDL_RenderFillRect(gRenderer, &playerRect);

            // Set render color to blue (for the enemy rectangle)
            SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);

            // Render the enemy rectangle
            SDL_RenderFillRect(gRenderer, &enemyRect);

            // Set render color to yellow (for the collectables)
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);

            // Render the collectables
            for (const auto& collectable : collectables)
            {
                SDL_RenderFillRect(gRenderer, &collectable);
            }

            // Update screen
            SDL_RenderPresent(gRenderer);

            // Add a delay to control the update frequency
            SDL_Delay(30); // Delay in milliseconds
        }
    }

    // Free resources and close SDL
    close();

    return 0;
}