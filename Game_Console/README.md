Pacman

Overview

    This is a terminal-based C maze game where players navigate through predefined levels to reach an exit while collecting bonuses and avoiding traps. The game uses ASCII characters for rendering and ANSI color codes for visual enhancement. It features multiple levels, a scoring system, and a high score tracker.
    Features

Gameplay Mechanics:

    Navigate a player (@) through a maze to reach the exit (E).
    Collect bonuses (*) for +50 points and -5 moves.
    Avoid traps (T) that deduct 100 points.
    Score decreases by 10 per move, with a speed bonus for quick level completion.
    Game over if score reaches zero.


Levels:

    Three predefined levels (Easy, Medium, Hard) with increasing complexity.
    Walls (#) block movement, and empty spaces allow navigation.


Visuals:

    ASCII-based map with colored elements:
    Player: Green @
    Exit: Blue E
    Bonus: Magenta *
    Trap: Red T
    Walls: Cyan █
    
    
    Status bar showing level, moves, score, time, bonuses, traps, and controls.
    Menu and transition animations using ANSI colors.


Scoring:

    Start with 1000 points per level.
    Bonuses add 50 points and reduce moves by 5.
    Traps deduct 100 points.
    Each move costs 10 points.
    Speed bonus: 1000 / (timeTaken + 1) added upon reaching the exit.
    High score persists across sessions.


Controls:

    W/w: Move up
    S/s: Move down
    A/a: Move left
    D/d: Move right
    U/u: Move up-left
    I/i: Move up-right
    J/j: Move down-left
    K/k: Move down-right
    Q/q: Quit game
    R/r: Retry level on game over


UI:

    Title screen with options to start, view controls, or exit.
    Level transition animation with loading dots.
    Game over screen with retry or menu options.
    Completion screen after all levels with final score and high score.



Prerequisites

    C Compiler: A compiler supporting C (e.g., gcc).
    Operating System: Designed for Unix-like systems (Linux, macOS) due to termios.h and getchar for input. Windows compatibility may require modifications (e.g., using conio.h for input).
    Terminal: A terminal supporting ANSI color codes.

Compilation and Running

    Compile the Code:make
    Run the Game:./labyrinth_adventure



How to Play

    Launch the game to see the title screen.
    Select:
    1 to start the game.
    2 to view controls.
    3 to exit.
    
    
    Navigate through each level using movement keys.
    Reach the exit (E) to complete a level.
    Collect bonuses (*) and avoid traps (T).
    If score reaches zero, choose to retry (R) or return to the menu.
    Complete all levels to see the final score and high score.
    Press any key to replay or Q to quit.

Code Structure

    Main File: Contains the game logic, level definitions, and rendering functions.
    Level Struct: Defines each level with:
    2D map array (char map[MAX_HEIGHT][MAX_WIDTH]).
    Dimensions (width, height).
    Player and exit coordinates.
    Moves, score, time taken, bonus, and trap counts.


Key Functions:

    getInput(): Reads single keypress without requiring Enter (Unix-specific).
    renderTitleScreen(): Displays the main menu.
    showControls(): Shows control instructions.
    animateTransition(): Displays level loading animation.
    drawMap(): Renders the maze and status bar.
    movePlayer(): Handles player movement, bonuses, traps, and scoring.
    initLevel(): Resets level state and counts bonuses/traps.
    

Game Loop:

    Manages level progression, input handling, and game state updates.
    Tracks time for scoring and displays game over or completion messages.


Future Improvements

    Add Windows compatibility using conio.h or a library like ncurses.
    Support dynamic terminal resizing for better map display.
    Add more levels or a level generator for variety.
    Implement sound effects using a cross-platform audio library.
    Add save/load functionality for high scores.
    Include enemies or timed obstacles for increased challenge.
    Improve input handling to support simultaneous keypresses.



