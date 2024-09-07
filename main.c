#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BAR_WIDTH = 40;
int values[] = {0, 1, 10, 3, 20, 40, 22, 3, 4, 5, 24, 17, 38, 12, 19};  // Array to be sorted
int arraySize = sizeof(values) / sizeof(values[0]);

int sortingStates[100][15];  // Array to store up to 100 steps of sorting (adjustable)
int currentStep = 0;         // To track the current step
int totalSteps = 0;          // Total number of recorded sorting steps

// Function to render a bar at a given x position with the specified height
void renderBar(SDL_Renderer* renderer, int x, int height, bool isActive) {
    SDL_Rect bar = { x, SCREEN_HEIGHT - height * 10, BAR_WIDTH, height * 10 };
    if (isActive) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Highlight active bar in red
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Normal bar in blue
    }
    SDL_RenderFillRect(renderer, &bar);
}

// Function to render the entire array
void renderArray(SDL_Renderer* renderer, int* array, int size, int active1, int active2) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White background
    SDL_RenderClear(renderer);

    for (int i = 0; i < size; i++) {
        int xPosition = i * (BAR_WIDTH + 5);
        bool isActive = (i == active1 || i == active2);
        renderBar(renderer, xPosition, array[i], isActive);
    }

    SDL_RenderPresent(renderer);  // Present the changes to the screen
}

// Save the current state of the array
void saveSortingState(int* array, int size) {
    memcpy(sortingStates[totalSteps], array, size * sizeof(int));  // Copy current array state
    totalSteps++;
}

// Bubble Sort with state saving
void bubbleSortAndSaveSteps(SDL_Renderer* renderer, int* array, int size) {
    totalSteps = 0;  // Reset the number of steps recorded

    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            // Save state before comparison
            saveSortingState(array, size);
            renderArray(renderer, array, size, j, j+1);
            SDL_Delay(500);  // Delay to simulate visualization speed

            // Swap if needed
            if (array[j] > array[j+1]) {
                int temp = array[j];
                array[j] = array[j+1];
                array[j+1] = temp;
            }

            // Save state after the swap
            saveSortingState(array, size);
        }
    }
}

// Main function to handle key presses and rendering
int main(int argc, char* args[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Sorting Algorithm Visualization", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Perform bubble sort and save states for each step
    bubbleSortAndSaveSteps(renderer, values, arraySize);

    // Event handling
    bool quit = false;
    SDL_Event event;

    // Initially render the first step
    renderArray(renderer, sortingStates[currentStep], arraySize, -1, -1);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;  // Handle window close event
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:  // Press ESC to quit
                        quit = true;
                        break;
                    case SDLK_n:  // Press 'n' to move to the next step
                        if (currentStep < totalSteps - 1) {
                            currentStep++;
                            renderArray(renderer, sortingStates[currentStep], arraySize, -1, -1);
                        }
                        break;
                    case SDLK_b:  // Press 'b' to move back to the previous step
                        if (currentStep > 0) {
                            currentStep--;
                            renderArray(renderer, sortingStates[currentStep], arraySize, -1, -1);
                        }
                        break;
                }
            }
        }

        // Delay to avoid consuming too much CPU
        SDL_Delay(16);  // 60 FPS cap (1000 ms / 60 = ~16 ms)
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

