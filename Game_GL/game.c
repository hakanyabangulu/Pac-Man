#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

// Harita boyutları ve sabitler
#define MAX_WIDTH 30
#define MAX_HEIGHT 20
#define MAX_LEVELS 5
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define FPS 60
#define TIME_LIMIT 120.0f
#define ENEMY_SPEED 0.1f
#define PLAYER_SPEED 1.0f
#define MAX_LIVES 3
#define NOTIFICATION_DURATION 2.0f

// Oyun durumu
typedef struct {
    char map[MAX_HEIGHT][MAX_WIDTH];
    int width, height;
    float playerX, playerY;
    int exitX, exitY;
    int moves;
    int score;
    float timeTaken;
    int bonusCount;
    int trapCount;
    int powerupCount;
    int enemyCount;
    int lives;
    float multiplier;
} Level;

// Düşman yapısı
typedef struct {
    float x, y;
    int active;
    float spawnTime;
} Enemy;

// Bildirim yapısı
typedef struct {
    char text[100];
    float r, g, b;
    float time;
} Notification;

// Oyun durumu
static Level levels[MAX_LEVELS];
static Enemy enemies[MAX_LEVELS][10];
static int currentLevel = 0;
static int highScore = 0;
static int gameState = 0; // 0: Menü, 1: Oyun, 2: Kontroller, 3: Seviye Tamamlandı, 4: Oyun Bitti, 5: Oyun Tamamlandı, 6: Ayarlar, 7: Duraklat
static time_t startTime;
static int needsRedraw = 1;
static float cameraX = 0.0f, cameraY = 0.0f;
static float animTime = 0.0f;
static int particles[100][4]; // x, y, yaşam süresi, tip
static GLuint mapDisplayList;
static float transitionAlpha = 0.0f;
static int hardMode = 0;
static float playerSpeed = PLAYER_SPEED;
static Notification notification = {"", 1.0f, 1.0f, 1.0f, 0.0f};
static float shakeTime = 0.0f;
static float lastFrameTime = 0.0f;
static int keys[512] = {0}; // Daha büyük dizi, özel tuşlar için

// Seviye tanımlamaları
void initLevels() {
    levels[0] = (Level){
        .map = {
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', '@', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '*', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#', '#', '#', '#', '#', ' ', '#', '#', '#', ' ', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '#'},
            {'#', ' ', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
        },
        .width = 30, .height = 11, .playerX = 1.0f, .playerY = 1.0f, .exitX = 28, .exitY = 9,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 2, .trapCount = 1, .powerupCount = 0, .enemyCount = 0
    };
    levels[1] = (Level){
        .map = {
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', '@', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '*', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 'E', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
        },
        .width = 30, .height = 13, .playerX = 1.0f, .playerY = 1.0f, .exitX = 28, .exitY = 10,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 2, .trapCount = 1, .powerupCount = 1, .enemyCount = 1
    };
    levels[2] = (Level){
        .map = {
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', '@', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '*', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 'E', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
        },
        .width = 30, .height = 17, .playerX = 1.0f, .playerY = 1.0f, .exitX = 28, .exitY = 14,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 3, .trapCount = 1, .powerupCount = 1, .enemyCount = 2
    };
    levels[3] = (Level){
        .map = {
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', '@', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '*', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#', '#', '#', '#', '#', ' ', '#', '#', '#', ' ', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '#'},
            {'#', ' ', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 'P', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
        },
        .width = 30, .height = 11, .playerX = 1.0f, .playerY = 1.0f, .exitX = 28, .exitY = 9,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 2, .trapCount = 1, .powerupCount = 1, .enemyCount = 2
    };
    levels[4] = (Level){
        .map = {
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
            {'#', '@', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '*', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'T', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '*', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 'P', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'X', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 'E', '#'},
            {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
            {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
        },
        .width = 30, .height = 13, .playerX = 1.0f, .playerY = 1.0f, .exitX = 28, .exitY = 10,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 2, .trapCount = 1, .powerupCount = 1, .enemyCount = 3
    };
    for (int i = 0; i < MAX_LEVELS; i++) {
        for (int j = 0; j < 10; j++) {
            enemies[i][j].active = 0;
            enemies[i][j].spawnTime = 0.0f;
        }
    }
    enemies[1][0] = (Enemy){5.0f, 5.0f, 1, 0.0f};
    enemies[2][0] = (Enemy){10.0f, 10.0f, 1, 0.0f};
    enemies[2][1] = (Enemy){15.0f, 5.0f, 1, 0.0f};
    enemies[3][0] = (Enemy){20.0f, 3.0f, 1, 0.0f};
    enemies[3][1] = (Enemy){7.0f, 7.0f, 1, 0.0f};
    enemies[4][0] = (Enemy){12.0f, 8.0f, 1, 0.0f};
    enemies[4][1] = (Enemy){18.0f, 4.0f, 1, 0.0f};
    enemies[4][2] = (Enemy){25.0f, 6.0f, 1, 0.0f};
}

// Seviyeyi başlat
void initLevel(Level *level) {
    level->moves = 0;
    level->score = 1000;
    level->timeTaken = 0;
    level->lives = MAX_LIVES;
    level->multiplier = 1.0f;
    level->bonusCount = 0;
    level->trapCount = 0;
    level->powerupCount = 0;
    level->enemyCount = 0;
    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            if (level->map[y][x] == '*') level->bonusCount++;
            if (level->map[y][x] == 'T') level->trapCount++;
            if (level->map[y][x] == 'P') level->powerupCount++;
            if (level->map[y][x] == 'X') level->enemyCount++;
        }
    }
    startTime = time(NULL);
    for (int i = 0; i < 100; i++) particles[i][3] = 0;
    notification.time = 0.0f;
    transitionAlpha = 1.0f;
    glNewList(mapDisplayList, GL_COMPILE);
    float cellWidth = 2.0f / level->width;
    float cellHeight = 2.0f / level->height;
    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            float px = -1.0f + x * cellWidth;
            float py = 1.0f - (y + 1) * cellHeight;
            char c = level->map[y][x];
            if (c == '#') {
                glColor3f(0.3f, 0.3f, 0.3f);
                glBegin(GL_QUADS);
                glVertex2f(px, py);
                glVertex2f(px + cellWidth, py);
                glVertex2f(px + cellWidth, py + cellHeight);
                glVertex2f(px, py + cellHeight);
                glEnd();
            } else if (c == 'E') {
                glColor3f(0.0f, 0.8f, 0.4f);
                glBegin(GL_QUADS);
                glVertex2f(px, py);
                glVertex2f(px + cellWidth, py);
                glVertex2f(px + cellWidth, py + cellHeight);
                glVertex2f(px, py + cellHeight);
                glEnd();
            }
        }
    }
    glEndList();
}

// Yüksek skoru kaydet/yükle
void saveHighScore() {
    FILE *f = fopen("highscore.txt", "w");
    if (f) {
        fprintf(f, "%d\n", highScore);
        fclose(f);
    }
}

void loadHighScore() {
    FILE *f = fopen("highscore.txt", "r");
    if (f) {
        if (fscanf(f, "%d", &highScore) != 1) {
            highScore = 0;
        }
        fclose(f);
    }
}

// Metin çizimi
void drawText(float x, float y, const char *text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Kare çizimi
void drawSquare(float x, float y, float r, float g, float b, float scale, float alpha) {
    glColor4f(r, g, b, alpha);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + scale, y);
    glVertex2f(x + scale, y + scale);
    glVertex2f(x, y + scale);
    glEnd();
}

// Parçacık çizimi
void drawParticles() {
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 100; i++) {
        if (particles[i][3] > 0) {
            float t = particles[i][3] / 30.0f;
            if (particles[i][2] == 0) glColor4f(1.0f, 0.9f, 0.0f, t); // Bonus
            else if (particles[i][2] == 1) glColor4f(0.8f, 0.1f, 0.1f, t); // Tuzak
            else glColor4f(0.0f, 0.8f, 0.8f, t); // Güçlendirici
            glVertex2f(particles[i][0], particles[i][1]);
            particles[i][3]--;
        }
    }
    glEnd();
}

// Vignette efekti
void drawVignette() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f); glVertex2f(1.0f, -0.8f); glVertex2f(-1.0f, -0.8f);
    glVertex2f(-1.0f, 1.0f); glVertex2f(1.0f, 1.0f); glVertex2f(1.0f, 0.8f); glVertex2f(-1.0f, 0.8f);
    glVertex2f(-1.0f, -1.0f); glVertex2f(-0.8f, -1.0f); glVertex2f(-0.8f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f); glVertex2f(0.8f, -1.0f); glVertex2f(0.8f, 1.0f); glVertex2f(1.0f, 1.0f);
    glEnd();
    glDisable(GL_BLEND);
}

// Harita çizimi
void drawMap(Level *level) {
    float cellWidth = 2.0f / level->width;
    float cellHeight = 2.0f / level->height;
    glCallList(mapDisplayList);
    float blink = 0.8f + 0.2f * sin(animTime * 5.0f);
    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            float px = -1.0f + x * cellWidth;
            float py = 1.0f - (y + 1) * cellHeight;
            char c = level->map[y][x];
            if (c == '*') {
                glPushMatrix();
                glTranslatef(px + cellWidth / 2, py + cellHeight / 2, 0);
                glRotatef(animTime * 90.0f, 0, 0, 1);
                glTranslatef(-cellWidth / 2, -cellHeight / 2, 0);
                drawSquare(0, 0, 1.0f, 0.9f, 0.0f, cellWidth, 0.8f + 0.2f * sin(animTime * 2.0f));
                glPopMatrix();
            } else if (c == 'T') {
                drawSquare(px, py, 0.8f, 0.1f, 0.1f, cellWidth, blink);
            } else if (c == 'P') {
                drawSquare(px, py, 0.0f, 0.8f, 0.8f, cellWidth, 0.8f + 0.2f * cos(animTime * 3.0f));
            } else if (c == 'X') {
                drawSquare(px, py, 0.8f, 0.1f, 0.1f, cellWidth, blink);
            }
        }
    }
    float px = -1.0f + level->playerX * cellWidth;
    float py = 1.0f - (level->playerY + 1) * cellHeight;
    float bob = 0.05f * sin(animTime * 10.0f);
    drawSquare(px, py + bob, 1.0f, 0.6f, 0.0f, cellWidth, 1.0f);
    for (int i = 0; i < level->enemyCount; i++) {
        if (enemies[currentLevel][i].active) {
            float scale = enemies[currentLevel][i].spawnTime < 1.0f ? enemies[currentLevel][i].spawnTime : 1.0f;
            px = -1.0f + enemies[currentLevel][i].x * cellWidth;
            py = 1.0f - (enemies[currentLevel][i].y + 1) * cellHeight;
            drawSquare(px + (1.0f - scale) * cellWidth / 2, py + (1.0f - scale) * cellHeight / 2,
                       0.8f, 0.1f, 0.1f, cellWidth * scale, scale);
        }
    }
}

// Ekran çizimi
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    Level *level = &levels[currentLevel];
    float shakeX = shakeTime > 0 ? 0.02f * sin(animTime * 50.0f) : 0.0f;
    float shakeY = shakeTime > 0 ? 0.02f * cos(animTime * 50.0f) : 0.0f;
    glPushMatrix();
    glTranslatef(shakeX, shakeY, 0);

    if (gameState == 0) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        float pulse = 0.9f + 0.1f * sin(animTime * 2.0f);
        drawText(-0.4f, 0.6f, "LABIRENT MACERASI", pulse, pulse * 0.9f, 0.0f);
        drawText(-0.4f, 0.4f, "@: Oyuncu | E: Cikis | *: Bonus (+50 puan)", 0.8f, 0.8f, 0.8f);
        drawText(-0.4f, 0.3f, "T: Tuzak (-1 can) | P: Guclendirici | X: Dusman", 0.8f, 0.8f, 0.8f);
        char scoreText[50];
        sprintf(scoreText, "Yuksek Skor: %d", highScore);
        drawText(-0.4f, 0.2f, scoreText, 0.8f, 0.8f, 0.8f);
        drawText(-0.4f, 0.0f, "1. Oyuna Basla", 1.0f, 0.9f, 0.0f);
        drawText(-0.4f, -0.1f, "2. Kontroller", 1.0f, 0.9f, 0.0f);
        drawText(-0.4f, -0.2f, "3. Ayarlar", 1.0f, 0.9f, 0.0f);
        drawText(-0.4f, -0.3f, "4. Cikis", 1.0f, 0.9f, 0.0f);
    } else if (gameState == 1 || gameState == 7) {
        glPushMatrix();
        glTranslatef(-cameraX, -cameraY, 0);
        drawMap(level);
        drawParticles();
        glPopMatrix();
        drawVignette();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f); glVertex2f(1.0f, -0.8f); glVertex2f(-1.0f, -0.8f);
        glEnd();
        char status[100];
        sprintf(status, "Seviye: %d | Can: %d | Skor: %d | Sure: %.1f/%.1f | Bonus: %d | Carp: %.1fx",
                currentLevel + 1, level->lives, level->score, level->timeTaken, TIME_LIMIT, level->bonusCount, level->multiplier);
        drawText(-0.95f, -0.95f, status, 1.0f, 1.0f, 1.0f);
        char highScoreText[50];
        sprintf(highScoreText, "Yuksek Skor: %d", highScore);
        drawText(-0.95f, -0.85f, highScoreText, 1.0f, 1.0f, 1.0f);
        if (notification.time > 0) {
            drawText(-0.4f, 0.0f, notification.text, notification.r, notification.g, notification.b);
        }
        if (gameState == 7) {
            drawText(-0.4f, 0.2f, "OYUN DURAKLATILDI", 1.0f, 0.9f, 0.0f);
            drawText(-0.4f, 0.0f, "1. Devam Et", 1.0f, 0.9f, 0.0f);
            drawText(-0.4f, -0.1f, "2. Yeniden Baslat", 1.0f, 0.9f, 0.0f);
            drawText(-0.4f, -0.2f, "3. Menuye Don", 1.0f, 0.9f, 0.0f);
        }
        glDisable(GL_BLEND);
    } else if (gameState == 2) {
        drawText(-0.4f, 0.6f, "KONTROLLER", 1.0f, 0.9f, 0.0f);
        drawText(-0.4f, 0.4f, "W/Yukari Ok: Yukari", 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, 0.3f, "S/Asagi Ok: Asagi", 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, 0.2f, "A/Sol Ok: Sola", 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, 0.1f, "D/Sag Ok: Saga", 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, 0.0f, "P: Duraklat", 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, -0.1f, "Q: Cikis", 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, -0.3f, "Geri donmek icin bir tusa basin...", 1.0f, 0.9f, 0.0f);
    } else if (gameState == 3) {
        drawText(-0.4f, 0.1f, "Seviye Tamamlandi!", 0.0f, 0.8f, 0.4f);
        char completeText[100];
        sprintf(completeText, "Skor: %d | Sure: %.1f sn", level->score, level->timeTaken);
        drawText(-0.4f, 0.0f, completeText, 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, -0.2f, "Devam etmek icin bir tusa basin...", 1.0f, 0.9f, 0.0f);
    } else if (gameState == 4) {
        drawText(-0.4f, 0.1f, "Oyun Bitti! Canlarin bitti.", 0.8f, 0.1f, 0.1f);
        drawText(-0.4f, 0.0f, "R: Tekrar Dene | Baska tus: Menuye Don", 1.0f, 0.9f, 0.0f);
    } else if (gameState == 5) {
        drawText(-0.4f, 0.1f, "Tebrikler! Oyunu Bitirdin!", 0.0f, 0.8f, 0.4f);
        char finalScoreText[50];
        sprintf(finalScoreText, "Son Skor: %d | Yuksek Skor: %d", level->score, highScore);
        drawText(-0.4f, 0.0f, finalScoreText, 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, -0.2f, "Tekrar oynamak icin bir tusa basin...", 1.0f, 0.9f, 0.0f);
    } else if (gameState == 6) {
        drawText(-0.4f, 0.6f, "AYARLAR", 1.0f, 0.9f, 0.0f);
        char modeText[50];
        sprintf(modeText, "1. Zor Mod: %s", hardMode ? "Acik" : "Kapali");
        drawText(-0.4f, 0.4f, modeText, 1.0f, 1.0f, 1.0f);
        char speedText[50];
        sprintf(speedText, "2. Oyuncu Hizi: %.1f", playerSpeed);
        drawText(-0.4f, 0.3f, speedText, 1.0f, 1.0f, 1.0f);
        drawText(-0.4f, 0.1f, "3. Menuye Don", 1.0f, 0.9f, 0.0f);
    }
    if (transitionAlpha > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, transitionAlpha);
        glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f); glVertex2f(1.0f, -1.0f); glVertex2f(1.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
        glEnd();
        glDisable(GL_BLEND);
    }
    glPopMatrix();
    glutSwapBuffers();
}

// Oyuncuyu hareket ettir
int movePlayer(Level *level, float dx, float dy) {
    int newX = (int)level->playerX + (dx > 0 ? 1 : dx < 0 ? -1 : 0);
    int newY = (int)level->playerY + (dy > 0 ? 1 : dy < 0 ? -1 : 0);

    if (newX < 0 || newX >= level->width || newY < 0 || newY >= level->height || level->map[newY][newX] == '#') {
        
        return 0;
    }

    if (level->map[newY][newX] == '*') {
        level->score += (int)(50 * level->multiplier);
        level->bonusCount--;
        level->multiplier += 0.1f;
        sprintf(notification.text, "Bonus! +%d Puan", (int)(50 * level->multiplier));
        notification.r = 1.0f; notification.g = 0.9f; notification.b = 0.0f;
        notification.time = NOTIFICATION_DURATION;
        for (int i = 0; i < 20; i++) {
            if (particles[i][3] == 0) {
                particles[i][0] = newX + (rand() % 100) / 100.0f;
                particles[i][1] = newY + (rand() % 100) / 100.0f;
                particles[i][2] = 0;
                particles[i][3] = 30;
            }
        }
    } else if (level->map[newY][newX] == 'T' && level->multiplier <= 1.5f) {
        level->lives--;
        level->trapCount--;
        level->multiplier = 1.0f;
        shakeTime = 0.5f;
        sprintf(notification.text, "Tuzak! -1 Can");
        notification.r = 0.8f; notification.g = 0.1f; notification.b = 0.1f;
        notification.time = NOTIFICATION_DURATION;
        for (int i = 0; i < 20; i++) {
            if (particles[i][3] == 0) {
                particles[i][0] = newX + (rand() % 100) / 100.0f;
                particles[i][1] = newY + (rand() % 100) / 100.0f;
                particles[i][2] = 1;
                particles[i][3] = 30;
            }
        }
    } else if (level->map[newY][newX] == 'P') {
        level->powerupCount--;
        level->multiplier = rand() % 2 ? 2.0f : 1.5f;
        sprintf(notification.text, level->multiplier > 1.5f ? "Hiz Artisi!" : "Dokunulmazlik!");
        notification.r = 0.0f; notification.g = 0.8f; notification.b = 0.8f;
        notification.time = NOTIFICATION_DURATION;
        for (int i = 0; i < 20; i++) {
            if (particles[i][3] == 0) {
                particles[i][0] = newX + (rand() % 100) / 100.0f;
                particles[i][1] = newY + (rand() % 100) / 100.0f;
                particles[i][2] = 2;
                particles[i][3] = 30;
            }
        }
    } else if (level->map[newY][newX] == 'X' && level->multiplier <= 1.5f) {
        level->lives--;
        level->multiplier = 1.0f;
        level->map[newY][newX] = ' ';
        level->enemyCount--;
        shakeTime = 0.5f;
        sprintf(notification.text, "Dusman! -1 Can");
        notification.r = 0.8f; notification.g = 0.1f; notification.b = 0.1f;
        notification.time = NOTIFICATION_DURATION;
    }

    level->map[(int)level->playerY][(int)level->playerX] = ' ';
    level->playerX = newX;
    level->playerY = newY;
    level->map[newY][newX] = '@';
    level->moves++;
    level->score = (level->score > 10) ? level->score - 10 : 0;
    
    return 1;
}

// Düşmanları hareket ettir
void moveEnemies(Level *level, float deltaTime) {
    float speed = ENEMY_SPEED * (1.0f + 0.2f * currentLevel) * (hardMode ? 1.5f : 1.0f);
    for (int i = 0; i < level->enemyCount; i++) {
        if (!enemies[currentLevel][i].active) continue;
        if (enemies[currentLevel][i].spawnTime < 1.0f) {
            enemies[currentLevel][i].spawnTime += deltaTime;
            continue;
        }
        float dx = level->playerX - enemies[currentLevel][i].x;
        float dy = level->playerY - enemies[currentLevel][i].y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < 5.0f) {
            dx /= dist; dy /= dist;
        } else {
            dx = ((rand() % 3) - 1) * 0.5f;
            dy = ((rand() % 3) - 1) * 0.5f;
        }
        float newX = enemies[currentLevel][i].x + dx * speed * deltaTime;
        float newY = enemies[currentLevel][i].y + dy * speed * deltaTime;
        int ix = (int)round(newX);
        int iy = (int)round(newY);
        if (ix >= 0 && ix < level->width && iy >= 0 && iy < level->height && level->map[iy][ix] != '#') {
            enemies[currentLevel][i].x = ix;
            enemies[currentLevel][i].y = iy;
            if (fabs(ix - level->playerX) < 0.5f && fabs(iy - level->playerY) < 0.5f && level->multiplier <= 1.5f) {
                level->lives--;
                enemies[currentLevel][i].active = 0;
                shakeTime = 0.5f;
                sprintf(notification.text, "Dusman! -1 Can");
                notification.r = 0.8f; notification.g = 0.1f; notification.b = 0.1f;
                notification.time = NOTIFICATION_DURATION;
            }
        }
    }
}

// Tuzakları hareket ettir
void moveTraps(Level *level, float deltaTime) {
    if (rand() % 100 < 2) {
        for (int y = 0; y < level->height; y++) {
            for (int x = 0; x < level->width; x++) {
                if (level->map[y][x] == 'T') {
                    int dx = (rand() % 3) - 1;
                    int dy = (rand() % 3) - 1;
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < level->width && ny >= 0 && ny < level->height &&
                        level->map[ny][nx] == ' ' && !(nx == (int)level->playerX && ny == (int)level->playerY)) {
                        level->map[y][x] = ' ';
                        level->map[ny][nx] = 'T';
                    }
                }
            }
        }
    }
}

// Kamera güncelleme
void updateCamera(Level *level, float deltaTime) {
    float targetX = level->playerX * 2.0f / level->width - 1.0f;
    float targetY = -level->playerY * 2.0f / level->height + 1.0f;
    cameraX += (targetX - cameraX) * 5.0f * deltaTime;
    cameraY += (targetY - cameraY) * 5.0f * deltaTime;
}

// Klavye girişi
void keyboardDown(unsigned char key, int x, int y) {
    if (gameState == 1) { // Sadece oyun modunda
        keys[key] = 1;
        
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (gameState == 1) { // Sadece oyun modunda
        keys[key] = 0;
        
    }
}

void specialKeys(int key, int x, int y) {
    keys[key + 256] = 1; // Özel tuşlar için ofset
   
}

void specialKeysUp(int key, int x, int y) {
    keys[key + 256] = 0;
    
}

void handleInput(Level *level, float deltaTime) {
    float dx = 0.0f, dy = 0.0f;
    if (keys['w'] || keys['W'] || keys[GLUT_KEY_UP + 256]) {
        dy -= 1.0f;
        
    }
    if (keys['s'] || keys['S'] || keys[GLUT_KEY_DOWN + 256]) {
        dy += 1.0f;
        
    }
    if (keys['a'] || keys['A'] || keys[GLUT_KEY_LEFT + 256]) {
        dx -= 1.0f;
       
    }
    if (keys['d'] || keys['D'] || keys[GLUT_KEY_RIGHT + 256]) {
        dx += 1.0f;
        
    }
    if (dx != 0 || dy != 0) {
        float len = sqrt(dx * dx + dy * dy);
        dx /= len; dy /= len;
        if (movePlayer(level, dx, dy)) {
           
            needsRedraw = 1;
            level->timeTaken = difftime(time(NULL), startTime);
            if (level->lives <= 0 || level->timeTaken >= TIME_LIMIT * (hardMode ? 0.8f : 1.0f)) {
                gameState = 4;
                transitionAlpha = 1.0f;
            } else if ((int)level->playerX == level->exitX && (int)level->playerY == level->exitY) {
                level->score += (int)(1000 / (level->timeTaken + 1));
                if (level->score > highScore) highScore = level->score;
                gameState = 3;
                transitionAlpha = 1.0f;
            }
        } else {
            printf("Hareket engellendi\n");
        }
    }
}

// Menü ve durum geçişleri
void keyboard(unsigned char key, int x, int y) {
    
    Level *level = &levels[currentLevel];

    if (gameState == 1 && (key == 'w' || key == 's' || key == 'a' || key == 'd' ||
                           key == 'W' || key == 'S' || key == 'A' || key == 'D')) {
        return; // Oyun sırasında hareket tuşlarını keyboardDown işlesin
    }

    if (gameState == 0) {
        switch (key) {
            case '1':
                printf("Oyun baslatiliyor: gameState = 1\n");
                gameState = 1;
                currentLevel = 0;
                initLevel(&levels[currentLevel]);
                transitionAlpha = 1.0f;
                needsRedraw = 1;
                break;
            case '2':
                printf("Kontroller ekrani: gameState = 2\n");
                gameState = 2;
                transitionAlpha = 1.0f;
                needsRedraw = 1;
                break;
            case '3':
                printf("Ayarlar ekrani: gameState = 6\n");
                gameState = 6;
                transitionAlpha = 1.0f;
                needsRedraw = 1;
                break;
            case '4':
                printf("Cikis\n");
                saveHighScore();
                exit(0);
                break;
        }
    } else if (gameState == 1) {
        if (key == 'p' || key == 'P') {
          
            gameState = 7;
            transitionAlpha = 0.5f;
            needsRedraw = 1;
        } else if (key == 'q' || key == 'Q') {
            printf("Cikis\n");
            saveHighScore();
            exit(0);
        }
    } else if (gameState == 2) {
       
        gameState = 0;
        transitionAlpha = 1.0f;
        needsRedraw = 1;
    } else if (gameState == 3) {
       
        currentLevel++;
        if (currentLevel >= MAX_LEVELS) {
            gameState = 5;
        } else {
            initLevel(&levels[currentLevel]);
            gameState = 1;
        }
        transitionAlpha = 1.0f;
        needsRedraw = 1;
    } else if (gameState == 4) {
        if (key == 'r' || key == 'R') {
            
            initLevel(level);
            gameState = 1;
            transitionAlpha = 1.0f;
        } else {
            
            gameState = 0;
            transitionAlpha = 1.0f;
        }
        needsRedraw = 1;
    } else if (gameState == 5) {
       
        currentLevel = 0;
        initLevel(&levels[currentLevel]);
        gameState = 1;
        transitionAlpha = 1.0f;
        needsRedraw = 1;
    } else if (gameState == 6) {
        if (key == '1') {
           
            hardMode = !hardMode;
            needsRedraw = 1;
        } else if (key == '2') {
           
            playerSpeed = playerSpeed == PLAYER_SPEED ? PLAYER_SPEED * 1.5f : PLAYER_SPEED;
            needsRedraw = 1;
        } else if (key == '3') {
            
            gameState = 0;
            transitionAlpha = 1.0f;
            needsRedraw = 1;
        }
    } else if (gameState == 7) {
        if (key == '1') {
           
            gameState = 1;
            transitionAlpha = 0.5f;
            needsRedraw = 1;
        } else if (key == '2') {
           
            initLevel(level);
            gameState = 1;
            transitionAlpha = 1.0f;
            needsRedraw = 1;
        } else if (key == '3') {
           
            gameState = 0;
            transitionAlpha = 1.0f;
            needsRedraw = 1;
        }
    }
    glutPostRedisplay();
}

// Zamanlayıcı
void timer(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    animTime += deltaTime;
    if (gameState == 1) {
        Level *level = &levels[currentLevel];
        level->timeTaken = difftime(time(NULL), startTime);
        handleInput(level, deltaTime);
        moveEnemies(level, deltaTime);
        moveTraps(level, deltaTime);
        updateCamera(level, deltaTime);
        if (notification.time > 0) {
            notification.time -= deltaTime;
            needsRedraw = 1;
        }
        if (shakeTime > 0) {
            shakeTime -= deltaTime;
            needsRedraw = 1;
        }
    }
    if (transitionAlpha > 0) {
        transitionAlpha -= deltaTime * 1.5f;
        if (transitionAlpha < 0) transitionAlpha = 0;
        needsRedraw = 1;
    }
    needsRedraw = 1;
    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, timer, 0);
}

// Ana fonksiyon
int main(int argc, char **argv) {
    srand(time(NULL));
    initLevels();
    loadHighScore();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Labirent Macerasi");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    mapDisplayList = glGenLists(1);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard); // Menü ve durum geçişleri için
    //glutKeyboardFunc(keyboardDown); // Oyun içi hareket için
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutTimerFunc(0, timer, 0);
    lastFrameTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    glutMainLoop();
    return 0;
}