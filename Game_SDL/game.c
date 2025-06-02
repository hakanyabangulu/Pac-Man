#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#define WIDTH 20
#define HEIGHT 10
#define CELL_SIZE 30
#define MAX_LEVELS 10
#define MAX_LIVES 3
#define GHOST_MOVE_INTERVAL 5

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position pos;
    int score;
    int lives;
} Player;

typedef struct {
    Position pos;
} Ghost;

typedef struct {
    char maze[HEIGHT][WIDTH + 1];
    int level;
    Player player;
    Ghost ghost;
    bool running;
    bool inStartScreen;
    bool inCutscene;
    bool gameOver;
    int ghostMoveCounter;
    SDL_Renderer* renderer;
    TTF_Font* font;
} GameState;

// Labirent dizisi (same as original, unchanged for brevity)
char mazes[MAX_LEVELS][HEIGHT][WIDTH + 1] = {
    {"||||||||||||||||||||", "|P . . . . . . .   |", "|  |||||   |||||   |", "|  |               |", "|  |  |||  |||     |", "|  |               |", "|  |||||  ||||||   |", "|  . . . . . . .   |", "|   |||||||||||    |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . . ||||  . . . |", "|      |  |  |||   |", "||||   |  |        |", "|      |  ||||||   |", "|  |||||           |", "|  |      ||||||   |", "|  . . ||||  . .   |", "|         ||||||   |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . ||||  . . . . |", "|    |     |||||   |", "|||| | |||     |   |", "|    | |  |||| |   |", "|  ||| |       |   |", "|      ||||    |   |", "| . . .  ||||  .   |", "||||||      ||||   |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . . . . ||||    |", "|  ||||    |  ||   |", "|  |  ||   |       |", "|  |  ||||||||     |", "|  ||      |       |", "|  ||||    |||||   |", "|  . . . . . . .   |", "|     |||||||||    |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . ||||| . . . . |", "|    |     |||||   |", "|||| | ||| |  ||   |", "|    | |   |  ||   |", "|  ||| | ||||      |", "|  |   ||||    ||  |", "|  . . .  |||| .   |", "|   ||||||||       |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . . . . . . ||  |", "|  |||||||||   ||  |", "|  |       |       |", "|  |||||   ||||||  |", "|      |   |       |", "|  |||||   |||||   |", "|  . . . . . . .   |", "|   |||||||||  ||  |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . ||||  . . . . |", "|  |    || |||||   |", "|     | ||||   ||  |", "||||  | || |   ||  |", "|  |  | || |       |", "|  |  |||| |||||   |", "|  . .  ||  . . .  |", "|   ||      ||||   |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . . . |||| . .  |", "|  ||||  |  ||||   |", "||||  |  |  ||  || |", "|  |  |  ||||| ||  |", "|  ||||  |  || ||  |", "|  || || |  || ||  |", "|  . . . .  || .   |", "|   |  ||||    ||  |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . ||||| . . . . |", "|  |||  || |||||   |", "|  |  | || |  ||   |", "|     | || |  ||   |", "||||  | || |||||   |", "|  |  ||||.|   ||  |", "|  . . . ||        |", "|   |||     ||     |", "||||||||||||||||||||"},
    {"||||||||||||||||||||", "|P . ||||| |||| .  |", "|  |||  || |  ||   |", "|       || |  ||   |", "||||  | || |||     |", "|  |||| || |  ||   |", "|  |  | ||    ||   |", "|  . . . |  |  .   |", "|   |||     ||||   |", "||||||||||||||||||||"}
};

void loadLevel(GameState* state) {
    for (int y = 0; y < HEIGHT; y++) {
        strcpy(state->maze[y], mazes[state->level][y]);
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (state->maze[y][x] == 'P' || state->maze[y][x] == 'G') {
                state->maze[y][x] = ' ';
            }
        }
    }
    state->player.pos.x = 1;
    state->player.pos.y = 1;
    state->ghost.pos.x = 18;
    state->ghost.pos.y = 1;
    state->maze[state->player.pos.y][state->player.pos.x] = 'P';
    state->maze[state->ghost.pos.y][state->ghost.pos.x] = 'G';
}

int countDots(GameState* state) {
    int dots = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (state->maze[y][x] == '.') dots++;
        }
    }
    return dots;
}

// Simple BFS for ghost pathfinding
int findPath(GameState* state, int startX, int startY, int targetX, int targetY, int* dx, int* dy) {
    bool visited[HEIGHT][WIDTH] = {0};
    Position queue[HEIGHT * WIDTH];
    int front = 0, rear = 0;
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    Position parent[HEIGHT][WIDTH];

    queue[rear++] = (Position){startX, startY};
    visited[startY][startX] = true;

    while (front < rear) {
        Position current = queue[front++];
        if (current.x == targetX && current.y == targetY) {
            // Backtrack to find the first move
            while (!(current.x == startX && current.y == startY)) {
                Position prev = parent[current.y][current.x];
                if (prev.x == startX && prev.y == startY) {
                    *dx = current.x - startX;
                    *dy = current.y - startY;
                    return 1;
                }
                current = prev;
            }
        }
        for (int i = 0; i < 4; i++) {
            int newX = current.x + directions[i][0];
            int newY = current.y + directions[i][1];
            if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT &&
                !visited[newY][newX] && state->maze[newY][newX] != '|') {
                queue[rear++] = (Position){newX, newY};
                visited[newY][newX] = true;
                parent[newY][newX] = current;
            }
        }
    }
    return 0; // No path found, ghost stays still
}

void movePlayer(GameState* state, char direction) {
    int newX = state->player.pos.x, newY = state->player.pos.y;
    if (direction == 'w') newY--;
    if (direction == 's') newY++;
    if (direction == 'a') newX--;
    if (direction == 'd') newX++;

    if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT || state->maze[newY][newX] == '|') {
        return;
    }
    if (state->maze[newY][newX] == '.') {
        state->player.score++;
        state->maze[newY][newX] = ' ';
        if (countDots(state) == 0) {
            state->level++;
            if (state->level >= MAX_LEVELS) {
                state->gameOver = true;
                return;
            }
            state->inCutscene = true;
            return;
        }
    }
    state->maze[state->player.pos.y][state->player.pos.x] = ' ';
    state->player.pos.x = newX;
    state->player.pos.y = newY;
    state->maze[state->player.pos.y][state->player.pos.x] = 'P';

    if (state->player.pos.x == state->ghost.pos.x && state->player.pos.y == state->ghost.pos.y) {
        state->player.lives--;
        if (state->player.lives <= 0) {
            state->gameOver = true;
            return;
        }
        loadLevel(state);
    }
}

void moveGhost(GameState* state) {
    state->ghostMoveCounter++;
    if (state->ghostMoveCounter < GHOST_MOVE_INTERVAL) return;
    state->ghostMoveCounter = 0;

    int dx, dy;
    if (findPath(state, state->ghost.pos.x, state->ghost.pos.y, state->player.pos.x, state->player.pos.y, &dx, &dy)) {
        int newX = state->ghost.pos.x + dx;
        int newY = state->ghost.pos.y + dy;
        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT &&
            (state->maze[newY][newX] == ' ' || state->maze[newY][newX] == 'P')) {
            state->maze[state->ghost.pos.y][state->ghost.pos.x] = ' ';
            state->ghost.pos.x = newX;
            state->ghost.pos.y = newY;
            state->maze[state->ghost.pos.y][state->ghost.pos.x] = 'G';
            if (state->ghost.pos.x == state->player.pos.x && state->ghost.pos.y == state->player.pos.y) {
                state->player.lives--;
                if (state->player.lives <= 0) {
                    state->gameOver = true;
                    return;
                }
                loadLevel(state);
            }
        }
    }
}

void drawStartScreen(GameState* state) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    SDL_Surface* titleSurface = TTF_RenderText_Solid(state->font, "PAC-MAN", (SDL_Color){255, 255, 0, 255});
    if (!titleSurface) return;
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(state->renderer, titleSurface);
    SDL_Rect titleRect = {WIDTH * CELL_SIZE / 2 - titleSurface->w / 2, 50, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(state->renderer, titleTexture, NULL, &titleRect);

    SDL_Surface* instSurface = TTF_RenderText_Solid(state->font, "W,A,S,D ile hareket et, Q ile cikis", (SDL_Color){255, 255, 255, 255});
    if (!instSurface) return;
    SDL_Texture* instTexture = SDL_CreateTextureFromSurface(state->renderer, instSurface);
    SDL_Rect instRect = {WIDTH * CELL_SIZE / 2 - instSurface->w / 2, 150, instSurface->w, instSurface->h};
    SDL_RenderCopy(state->renderer, instTexture, NULL, &instRect);

    SDL_Surface* startSurface = TTF_RenderText_Solid(state->font, "Baslamak icin herhangi bir tusa bas", (SDL_Color){255, 255, 255, 255});
    if (!startSurface) return;
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(state->renderer, startSurface);
    SDL_Rect startRect = {WIDTH * CELL_SIZE / 2 - startSurface->w / 2, 200, startSurface->w, startSurface->h};
    SDL_RenderCopy(state->renderer, startTexture, NULL, &startRect);

    SDL_RenderPresent(state->renderer);
    SDL_FreeSurface(titleSurface);
    SDL_FreeSurface(instSurface);
    SDL_FreeSurface(startSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(instTexture);
    SDL_DestroyTexture(startTexture);
}

void drawCutscene(GameState* state) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    char levelText[50];
    snprintf(levelText, sizeof(levelText), "Tebrikler! Seviye %d Gecildi", state->level);
    SDL_Surface* levelSurface = TTF_RenderText_Solid(state->font, levelText, (SDL_Color){0, 255, 0, 255});
    if (!levelSurface) return;
    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(state->renderer, levelSurface);
    SDL_Rect levelRect = {WIDTH * CELL_SIZE / 2 - levelSurface->w / 2, HEIGHT * CELL_SIZE / 2 - levelSurface->h / 2, levelSurface->w, levelSurface->h};
    SDL_RenderCopy(state->renderer, levelTexture, NULL, &levelRect);

    SDL_RenderPresent(state->renderer);
    SDL_Delay(2000);
    loadLevel(state);
    state->inCutscene = false;

    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);
}

void drawGameOver(GameState* state) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(state->font, state->level >= MAX_LEVELS ? "Tebrikler! Oyunu Kazandiniz!" : "Oyun Bitti!", (SDL_Color){255, 0, 0, 255});
    if (!gameOverSurface) return;
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(state->renderer, gameOverSurface);
    SDL_Rect gameOverRect = {WIDTH * CELL_SIZE / 2 - gameOverSurface->w / 2, HEIGHT * CELL_SIZE / 2 - gameOverSurface->h / 2, gameOverSurface->w, gameOverSurface->h};
    SDL_RenderCopy(state->renderer, gameOverTexture, NULL, &gameOverRect);

    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Puan: %d", state->player.score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(state->font, scoreText, (SDL_Color){255, 255, 255, 255});
    if (!scoreSurface) return;
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(state->renderer, scoreSurface);
    SDL_Rect scoreRect = {WIDTH * CELL_SIZE / 2 - scoreSurface->w / 2, HEIGHT * CELL_SIZE / 2 + 50, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(state->renderer, scoreTexture, NULL, &scoreRect);

    SDL_RenderPresent(state->renderer);
    SDL_Delay(3000);

    SDL_FreeSurface(gameOverSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(scoreTexture);
}

void drawMaze(GameState* state) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->renderer);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            if (state->maze[y][x] == '|') {
                SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(state->renderer, &rect);
            } else if (state->maze[y][x] == 'P') {
                SDL_SetRenderDrawColor(state->renderer, 255, 255, 0, 255);
                SDL_Rect pacman = {x * CELL_SIZE + CELL_SIZE / 4, y * CELL_SIZE + CELL_SIZE / 4, CELL_SIZE / 2, CELL_SIZE / 2};
                SDL_RenderFillRect(state->renderer, &pacman);
            } else if (state->maze[y][x] == '.') {
                SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
                SDL_Rect dot = {x * CELL_SIZE + CELL_SIZE / 2 - 5, y * CELL_SIZE + CELL_SIZE / 2 - 5, 10, 10};
                SDL_RenderFillRect(state->renderer, &dot);
            } else if (state->maze[y][x] == 'G') {
                SDL_SetRenderDrawColor(state->renderer, 255, 0, 0, 255);
                SDL_Rect ghost = {x * CELL_SIZE + CELL_SIZE / 4, y * CELL_SIZE + CELL_SIZE / 4, CELL_SIZE / 2, CELL_SIZE / 2};
                SDL_RenderFillRect(state->renderer, &ghost);
            }
        }
    }

    char hudText[100];
    snprintf(hudText, sizeof(hudText), "Puan: %d  Seviye: %d  Can: %d", state->player.score, state->level + 1, state->player.lives);
    SDL_Surface* hudSurface = TTF_RenderText_Solid(state->font, hudText, (SDL_Color){255, 255, 255, 255});
    if (!hudSurface) return;
    SDL_Texture* hudTexture = SDL_CreateTextureFromSurface(state->renderer, hudSurface);
    SDL_Rect hudRect = {10, HEIGHT * CELL_SIZE - 30, hudSurface->w, hudSurface->h};
    SDL_RenderCopy(state->renderer, hudTexture, NULL, &hudRect);

    SDL_RenderPresent(state->renderer);
    SDL_FreeSurface(hudSurface);
    SDL_DestroyTexture(hudTexture);
}

int main() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        fprintf(stderr, "SDL veya TTF başlatılamadı: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Pac-Man SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Pencere oluşturulamadı: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    GameState state = {
        .level = 0,
        .player = {.pos = {1, 1}, .score = 0, .lives = MAX_LIVES},
        .ghost = {.pos = {18, 1}},
        .running = true,
        .inStartScreen = true,
        .inCutscene = false,
        .gameOver = false,
        .ghostMoveCounter = 0,
        .renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED),
    };

    if (!state.renderer) {
        fprintf(stderr, "Renderer oluşturulamadı: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Try multiple font paths for cross-platform compatibility
    const char* fontPaths[] = {
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        NULL
    };
    state.font = NULL;
    for (int i = 0; fontPaths[i] && !state.font; i++) {
        state.font = TTF_OpenFont(fontPaths[i], 24);
    }
    if (!state.font) {
        fprintf(stderr, "Font yüklenemedi: %s\n", TTF_GetError());
        SDL_DestroyRenderer(state.renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    loadLevel(&state);
    SDL_Event event;

    while (state.running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                state.running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (state.inStartScreen) {
                    state.inStartScreen = false;
                } else if (!state.inCutscene && !state.gameOver) {
                    switch (event.key.keysym.sym) {
                        case SDLK_w: movePlayer(&state, 'w'); break;
                        case SDLK_s: movePlayer(&state, 's'); break;
                        case SDLK_a: movePlayer(&state, 'a'); break;
                        case SDLK_d: movePlayer(&state, 'd'); break;
                        case SDLK_q: state.running = false; break;
                    }
                }
            }
        }

        if (state.inStartScreen) {
            drawStartScreen(&state);
        } else if (state.inCutscene) {
            drawCutscene(&state);
        } else if (state.gameOver) {
            drawGameOver(&state);
            state.running = false;
        } else {
            moveGhost(&state);
            drawMaze(&state);
        }
        SDL_Delay(50);
    }

    TTF_CloseFont(state.font);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}