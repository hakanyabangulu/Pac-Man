#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <string.h>

// ANSI renk kodları
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

// Harita boyutları
#define MAX_WIDTH 30
#define MAX_HEIGHT 20

// Oyun seviyeleri
#define MAX_LEVELS 3

// Oyun durumu
typedef struct {
    char map[MAX_HEIGHT][MAX_WIDTH];
    int width, height;
    int playerX, playerY;
    int exitX, exitY;
    int moves;
    int score;
    double timeTaken;
    int bonusCount;
    int trapCount;
} Level;

// Yüksek skor
static int highScore = 0;

// Seviye tanımlamaları
Level levels[MAX_LEVELS] = {
    // Seviye 1: Kolay
    {
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
        .width = 30,
        .height = 11,
        .playerX = 1, .playerY = 1,
        .exitX = 28, .exitY = 9,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 2, .trapCount = 1
    },
    // Seviye 2: Orta
    {
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
        .width = 30,
        .height = 13,
        .playerX = 1, .playerY = 1,
        .exitX = 28, .exitY = 10,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 2, .trapCount = 1
    },
    // Seviye 3: Zor
    {
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
        .width = 30,
        .height = 17,
        .playerX = 1, .playerY = 1,
        .exitX = 28, .exitY = 14,
        .moves = 0, .score = 0, .timeTaken = 0, .bonusCount = 3, .trapCount = 1
    }
};

// Anlık tuş alma (getch alternatifi)
char getInput() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Başlık ekranı
void renderTitleScreen() {
    system("clear");
    printf(YELLOW "========================================\n" RESET);
    printf(GREEN "        LABİRENT MACERASI\n" RESET);
    printf(YELLOW "========================================\n" RESET);
    printf(CYAN "  @: Oyuncu | █: Duvar | E: Çıkış\n" RESET);
    printf(MAGENTA "  *: Bonus (+50 puan, -5 hareket)\n" RESET);
    printf(RED "  T: Tuzak (-100 puan)\n" RESET);
    printf(WHITE "\n  Yüksek Skor: %d\n" RESET, highScore);
    printf(YELLOW "\n1. Oyuna Başla\n2. Kontroller\n3. Çıkış\n" RESET);
    printf("Seçiminizi yapın (1-3): ");
}

// Kontrolleri göster
void showControls() {
    system("clear");
    printf(YELLOW "=== Kontroller ===\n" RESET);
    printf("W/w: Yukarı\n");
    printf("S/s: Aşağı\n");
    printf("A/a: Sola\n");
    printf("D/d: Sağa\n");
    printf("U/u: Sol-Yukarı\n");
    printf("I/i: Sağ-Yukarı\n");
    printf("J/j: Sol-Aşağı\n");
    printf("K/k: Sağ-Aşağı\n");
    printf("Q/q: Çıkış\n");
    printf("\nGeri dönmek için bir tuşa basın...");
    getInput();
}

// Seviye geçiş animasyonu
void animateTransition(int levelNum) {
    system("clear");
    printf(YELLOW "Seviye %d Yükleniyor...\n" RESET, levelNum);
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
    }
    printf("\n");
}

// Haritayı çiz
void drawMap(Level *level) {
    system("clear");
    // Üst çerçeve
    printf(YELLOW "╔");
    for (int x = 0; x < level->width * 2; x++) printf("═");
    printf("╗\n" RESET);
    
    // Harita
    for (int y = 0; y < level->height; y++) {
        printf(YELLOW "║" RESET);
        for (int x = 0; x < level->width; x++) {
            char c = level->map[y][x];
            if (c == '@') printf(GREEN "@ " RESET);
            else if (c == 'E') printf(BLUE "E " RESET);
            else if (c == '*') printf(MAGENTA "* " RESET);
            else if (c == 'T') printf(RED "T " RESET);
            else if (c == '#') printf(CYAN "█ " RESET);
            else printf("  ");
        }
        printf(YELLOW "║\n" RESET);
    }
    
    // Alt çerçeve
    printf(YELLOW "╚");
    for (int x = 0; x < level->width * 2; x++) printf("═");
    printf("╝\n" RESET);
    
    // Durum çubuğu
    printf(YELLOW "Seviye: %d | Hareket: %d | Skor: %d | Süre: %.1f sn | Bonus: %d | Tuzak: %d\n" RESET,
           (int)(level - levels + 1), level->moves, level->score, level->timeTaken, level->bonusCount, level->trapCount);
    printf(WHITE "Yüksek Skor: %d\n" RESET, highScore);
    printf(GREEN "Kontroller: W/A/S/D veya U/I/J/K - Hareket, Q - Çıkış\n" RESET);
}

// Oyuncuyu hareket ettir
int movePlayer(Level *level, int dx, int dy) {
    int newX = level->playerX + dx;
    int newY = level->playerY + dy;

    if (newX < 0 || newX >= level->width || newY < 0 || newY >= level->height)
        return 0;

    if (level->map[newY][newX] == '#') return 0;

    if (level->map[newY][newX] == '*') {
        level->score += 50;
        level->moves = (level->moves > 5) ? level->moves - 5 : 0;
        level->bonusCount--;
        printf("\a"); // Bonus sesi
    } else if (level->map[newY][newX] == 'T') {
        level->score -= 100;
        level->trapCount--;
        printf("\a"); // Tuzak sesi
    }

    level->map[level->playerY][level->playerX] = ' ';
    level->playerX = newX;
    level->playerY = newY;
    level->map[level->playerY][level->playerX] = '@';
    level->moves++;
    level->score = (level->score > 10) ? level->score - 10 : 0;
    return 1;
}

// Seviyeyi başlat
void initLevel(Level *level) {
    level->moves = 0;
    level->score = 1000;
    level->timeTaken = 0;
    // Bonus ve tuzak sayısını hesapla
    level->bonusCount = 0;
    level->trapCount = 0;
    for (int y = 0; y < level->height; y++) {
        for (int x = 0; x < level->width; x++) {
            if (level->map[y][x] == '*') level->bonusCount++;
            if (level->map[y][x] == 'T') level->trapCount++;
        }
    }
}

int main() {
    srand(time(NULL));
    int currentLevel = 0;

    while (1) {
        renderTitleScreen();
        char choice = getInput();
        if (choice == '3') break;
        if (choice == '2') {
            showControls();
            continue;
        }
        if (choice != '1') continue;

        // Seviye döngüsü
        for (currentLevel = 0; currentLevel < MAX_LEVELS; currentLevel++) {
            Level *level = &levels[currentLevel];
            initLevel(level);
            animateTransition(currentLevel + 1);
            time_t startTime = time(NULL);

            while (1) {
                drawMap(level);
                char input = getInput();
                int moved = 0;

                switch (input) {
                    case 'w': case 'W': moved = movePlayer(level, 0, -1); break;
                    case 's': case 'S': moved = movePlayer(level, 0, 1); break;
                    case 'a': case 'A': moved = movePlayer(level, -1, 0); break;
                    case 'd': case 'D': moved = movePlayer(level, 1, 0); break;
                    case 'u': case 'U': moved = movePlayer(level, -1, -1); break;
                    case 'i': case 'I': moved = movePlayer(level, 1, -1); break;
                    case 'j': case 'J': moved = movePlayer(level, -1, 1); break;
                    case 'k': case 'K': moved = movePlayer(level, 1, 1); break;
                    case 'q': case 'Q': return 0;
                }

                if (moved) {
                    level->timeTaken = difftime(time(NULL), startTime);
                    if (level->score <= 0) {
                        drawMap(level);
                        printf(RED "Oyun Bitti! Skorun sıfırlandı.\n" RESET);
                        printf("Tekrar denemek için 'R', menüye dönmek için başka bir tuşa basın...");
                        if (getInput() == 'R' || getInput() == 'r') {
                            initLevel(level);
                            startTime = time(NULL);
                            continue;
                        }
                        goto main_menu;
                    }
                    if (level->playerX == level->exitX && level->playerY == level->exitY) {
                        drawMap(level);
                        level->score += (int)(1000 / (level->timeTaken + 1)); // Hız bonusu
                        if (level->score > highScore) highScore = level->score;
                        printf(GREEN "Seviye %d tamamlandı! Skor: %d, Süre: %.1f sn\n" RESET,
                               currentLevel + 1, level->score, level->timeTaken);
                        printf("Devam etmek için bir tuşa basın...");
                        getInput();
                        break;
                    }
                }
            }
        }

        system("clear");
        printf(GREEN "Tebrikler! Tüm seviyeleri tamamladın!\n" RESET);
        printf("Son Skor: %d | Yüksek Skor: %d\n", levels[MAX_LEVELS-1].score, highScore);
        printf("Tekrar oynamak için bir tuşa basın, çıkmak için Q...");
        if (getInput() == 'q' || getInput() == 'Q') break;
    main_menu:
        continue;
    }

    system("clear");
    printf(YELLOW "Oyun bitti. Teşekkürler!\n" RESET);
    printf(WHITE "En Yüksek Skor: %d\n" RESET, highScore);
    return 0;
}