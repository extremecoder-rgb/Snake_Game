// Include necessary header files
#include <stdio.h>      // For input/output functions
#include <stdlib.h>     // For system() and rand()
#include <conio.h>      // For _kbhit() and _getch()
#include <windows.h>    // For Windows-specific functions
#include <time.h>       // For time functions used in random number generation

// Define game board dimensions
#define WIDTH 40        // Width of the game board
#define HEIGHT 20       // Height of the game board

// Structure to store x,y coordinates for snake body parts and food
struct Snake {
    int x;
    int y;
};

// Function declarations (prototypes)
void setup();          // Initialize game variables
void draw();           // Draw the game board
void input();          // Handle user input
void logic();          // Game logic processing
void gameOver();       // Handle game over scenario
void hideCursor();     // Hide the console cursor
void gotoxy(int x, int y);  // Move cursor to specific position

// Global variables
struct Snake snake[100];    // Array to store snake body coordinates (max 100 segments)
struct Snake food;          // Coordinates for food position
int snakeLength;           // Current length of the snake
int score;                 // Player's score
char direction;            // Current direction of snake movement
int gameRunning;          // Game state flag
int speed;                // Snake movement speed

// Function to hide the console cursor for better visuals
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Function to move console cursor to specific coordinates
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Initialize game state
void setup() {
    gameRunning = 1;           // Set game as running
    direction = 'r';           // Initial direction is right
    snakeLength = 1;           // Start with snake length of 1
    score = 0;                 // Initialize score to 0
    speed = 200;               // Set initial speed
    
    // Place snake in the middle of the screen
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    
    // Initialize random seed for food placement
    srand(time(NULL));
    // Place food at random position
    food.x = rand() % (WIDTH - 2) + 1;
    food.y = rand() % (HEIGHT - 2) + 1;
}

// Draw the game board and all elements
void draw() {
    gotoxy(0, 0);    // Move cursor to top-left corner
    int i, j;
    
    // Draw top border
    for (i = 0; i < WIDTH; i++)
        printf("#");
    printf("\n");
    
    // Draw middle area with snake, food, and side borders
    for (i = 0; i < HEIGHT - 2; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (j == 0 || j == WIDTH - 1)    // Draw side borders
                printf("#");
            else {
                int isSnakePart = 0;
                // Check if current position contains snake body
                for (int k = 0; k < snakeLength; k++) {
                    if (snake[k].x == j && snake[k].y == i + 1) {
                        printf("O");          // Draw snake body
                        isSnakePart = 1;
                        break;
                    }
                }
                if (!isSnakePart) {
                    if (food.x == j && food.y == i + 1)
                        printf("*");          // Draw food
                    else
                        printf(" ");          // Draw empty space
                }
            }
        }
        printf("\n");
    }
    
    // Draw bottom border
    for (i = 0; i < WIDTH; i++)
        printf("#");
    
    // Display score and controls
    printf("\nScore: %d", score);
    printf("\nControls: WASD or Arrow Keys | X to Exit");
}

// Handle keyboard input
void input() {
    if (_kbhit()) {   // Check if a key is pressed
        char key = _getch();
        if (key == -32) {  // Check for arrow keys (they send two codes)
            key = _getch(); // Get the actual arrow key code
            switch (key) {
                case 72: // Up arrow
                    if (direction != 's' && direction != 'd')  // Prevent 180-degree turns
                        direction = 'u';
                    break;
                case 80: // Down arrow
                    if (direction != 'u' && direction != 'd')
                        direction = 'd';
                    break;
                case 75: // Left arrow
                    if (direction != 'r' && direction != 'l')
                        direction = 'l';
                    break;
                case 77: // Right arrow
                    if (direction != 'l' && direction != 'r')
                        direction = 'r';
                    break;
            }
        } else {  // Handle WASD and X keys
            switch (key) {
                case 'a': case 'A':
                    if (direction != 'r' && direction != 'l')
                        direction = 'l';
                    break;
                case 'd': case 'D':
                    if (direction != 'l' && direction != 'r')
                        direction = 'r';
                    break;
                case 'w': case 'W':
                    if (direction != 's' && direction != 'd')
                        direction = 'u';
                    break;
                case 's': case 'S':
                    if (direction != 'u' && direction != 'd')
                        direction = 'd';
                    break;
                case 'x': case 'X':
                    gameRunning = 0;  // Exit game
                    break;
            }
        }
    }
}

// Process game logic
void logic() {
    // Store previous position of snake's head
    int prevX = snake[0].x;
    int prevY = snake[0].y;
    int prev2X, prev2Y;
    
    // Move snake's head based on direction
    switch (direction) {
        case 'l': snake[0].x--; break;
        case 'r': snake[0].x++; break;
        case 'u': snake[0].y--; break;
        case 'd': snake[0].y++; break;
    }
    
    // Move snake's body
    for (int i = 1; i < snakeLength; i++) {
        prev2X = snake[i].x;
        prev2Y = snake[i].y;
        snake[i].x = prevX;
        snake[i].y = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    
    // Check for wall collisions
    if (snake[0].x <= 0 || snake[0].x >= WIDTH - 1 ||
        snake[0].y <= 0 || snake[0].y >= HEIGHT - 1) {
        gameRunning = 0;
        return;
    }
    
    // Check for self collision
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameRunning = 0;
            return;
        }
    }
    
    // Check if snake ate food
    if (snake[0].x == food.x && snake[0].y == food.y) {
        score += 10;         // Increase score
        snakeLength++;       // Increase snake length
        
        // Generate new food position
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;
    }
}

// Display game over screen
void gameOver() {
    system("cls");    // Clear screen
    printf("\n\n\t\tGame Over!\n");
    printf("\t\tFinal Score: %d\n", score);
    printf("\n\t\tPress any key to exit...");
    _getch();         // Wait for key press
}

// Main function - program entry point
int main() {
    hideCursor();     // Hide the console cursor
    setup();          // Initialize the game
    
    // Main game loop
    while (gameRunning) {
        draw();       // Draw the game board
        input();      // Handle user input
        logic();      // Process game logic
        // Control game speed (increases with score)
        Sleep(200 - (score * 3));
    }
    
    gameOver();       // Show game over screen
    return 0;
}