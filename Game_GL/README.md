Pacman

Overview

    This is a 2D OpenGL-based maze game implemented in C, where players navigate through multiple levels to reach an exit while collecting bonuses, avoiding traps, and dodging enemies. The game uses GLUT for window and input management, featuring a graphical interface with animations, particle effects, and a dynamic camera system. It includes a scoring system, lives, power-ups, and a hard mode option.
    Features

Gameplay Mechanics:

    Navigate a player (orange square) to the exit (green square) in each level.
    Collect bonuses (*, +50 points with score multiplier).
    Avoid traps (T) and enemies (X), which deduct one life unless protected by a power-up.
    Power-ups (P) grant either a speed boost or temporary invincibility (multiplier > 1.5).
    Lives system: Start with 3 lives; game over if lives reach zero or time limit (120s, 96s in hard mode) expires.
    Score decreases by 10 per move; speed bonus (1000 / (timeTaken + 1)) added on level completion.
    High score saved to highscore.txt.


Levels:

    Five predefined levels with increasing complexity, bonuses, traps, power-ups, and enemies (0 to 3 per level).
    Walls (#) block movement; empty spaces allow navigation.


Visuals:

    2D grid-based rendering with OpenGL:
    Walls: Dark gray squares.
    Exit: Green square.
    Bonuses: Yellow rotating squares with pulsing opacity.
    Traps: Red blinking squares.
    Power-ups: Cyan pulsing squares.
    Enemies: Red scaling squares with spawn animation.
    Player: Orange square with bobbing animation.
    
    
    Particle effects for bonuses (yellow), traps (red), and power-ups (cyan).
    Vignette effect and screen shake on trap/enemy hits.
    Dynamic camera follows the player smoothly.
    Fade-in/out transitions between game states.


UI:

    Menu screens for start, controls, settings, level completion, game over, and game completion.
    Status bar showing level, lives, score, time, bonuses, and multiplier.
    Notifications for bonuses, traps, and power-ups with colored text.


Controls:

    W/w or Up Arrow: Move up.
    S/s or Down Arrow: Move down.
    A/a or Left Arrow: Move left.
    D/d or Right Arrow: Move right.
    P/p: Pause/unpause game.
    Q/q: Quit game.
    Menu selections:
    Main Menu: 1 (start), 2 (controls), 3 (settings), 4 (exit).
    Pause Menu: 1 (resume), 2 (restart), 3 (main menu).
    Game Over: R/r (retry), other keys (main menu).
    Settings: 1 (toggle hard mode), 2 (toggle player speed), 3 (main menu).




Dynamic Elements:

    Enemies move toward the player if within 5 units, otherwise randomly, with speed scaling by level and hard mode.
    Traps occasionally move to adjacent empty cells.
    Hard mode increases enemy speed and reduces time limit.
    Player speed toggle (1.0 or 1.5).
    Score multiplier increases with bonuses and resets on trap/enemy hits unless invincible.



Prerequisites

    OpenGL and GLUT: Ensure OpenGL and FreeGLUT are installed.
    On Ubuntu: sudo apt-get install freeglut3-dev
    On macOS: brew install freeglut
    On Windows: Install FreeGLUT and configure with your IDE (e.g., Visual Studio).
    
    
    C Compiler: A compiler supporting C (e.g., gcc).
    File System: Write access for saving high scores to highscore.txt.

Compilation and Running

    Install Dependencies:

    Ensure OpenGL and FreeGLUT are installed.


    Compile the Code: make
    
    
    Run the Game:./labyrinth_adventure



How to Play

    Launch the game to see the main menu.
    Select:
    1 to start the game.
    2 to view controls.
    3 to adjust settings (hard mode, player speed).
    4 to exit.


    Navigate through each level to reach the exit (E).
    Collect bonuses (*), avoid traps (T) and enemies (X), and use power-ups (P).
    If lives reach zero or time expires, retry (R) or return to the menu.
    Complete all levels to see the final score and high score.
    Press any key to replay or Q to quit.

Code Structure

    Main File: Contains game logic, level definitions, and OpenGL rendering.
    Level Struct: Defines each level with:
    2D map array (char map[MAX_HEIGHT][MAX_WIDTH]).
    Dimensions, player/exit coordinates, moves, score, time, counts for bonuses/traps/power-ups/enemies, lives, and multiplier.
    
    
    Enemy Struct: Tracks enemy position, activity, and spawn animation.
    Notification Struct: Manages temporary on-screen messages.
    Key Functions:
    initLevels(): Defines five levels with enemies.
    initLevel(): Resets level state, counts items, and compiles map display list.
    saveHighScore()/loadHighScore(): Manages high score persistence.
    drawText(): Renders text using GLUT bitmap fonts.
    drawSquare(): Draws colored squares for game elements.
    drawParticles(): Renders particle effects.
    drawVignette(): Adds screen border fade effect.
    drawMap(): Renders the maze with animations.
    movePlayer(): Handles player movement, item interactions, and scoring.
    moveEnemies(): Updates enemy positions toward player or randomly.
    moveTraps(): Occasionally relocates traps.
    updateCamera(): Smoothly tracks player position.
    handleInput(): Processes continuous movement input.
    keyboard(): Manages menu and state transitions.
    timer(): Updates game state at 60 FPS.


Game Loop:

    Uses GLUT’s timer for updates and redraws.
    Manages game states: menu (0), game (1), controls (2), level complete (3), game over (4), game complete (5), settings (6), pause (7).

Future Improvements

    Add texture support for walls and items.
    Implement sound effects using an audio library (e.g., OpenAL).
    Improve enemy AI with better pathfinding (e.g., A* algorithm).
    Add more power-up types (e.g., extra life, time extension).
    Support custom level loading from files.
    Enhance text rendering with a modern font library.
    Optimize rendering by reducing redundant glutPostRedisplay calls.
    Fix input handling to ensure smooth continuous movement.



