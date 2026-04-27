#include "raylib.h"
#include <stdlib.h> // rand and srand()
#include <time.h>   // constantly random no. (assists rand func.)
#include <fstream>
#include <iostream>
#include <string>
using namespace std;


const int grid_length = 10;
const int Battle_mode_cell_size = 45;
const int fleet_placement_cell_size = 60;

// both grids
int playerGrid[grid_length][grid_length] = { 0 };
int computerGrid[grid_length][grid_length] = { 0 };
string names[3];     // names in ldrbord
int scores[3];       // socres in ldrbord
int playerFleetSizes[5] = { 5, 4, 3, 3, 2 };
int computerFleetSizes[5] = { 5, 4, 3, 3, 2 };


int screen = 0;              // 0:Main Menu, 1:Fleet plcmnt, 2:Battle, 3:Game Over
int shipsPlaced = 0;         // No. of ships placed by the player
int currentfleetSize = 5;
int playerShipsRemaining = 5;
int computerShipsRemaining = 5;
bool horizontal = true;    // true if fleet is horizontal
int playerScore = 100;
int temp_screen = screen;
string name = "";
bool won = true;
bool yayPlayed = false;
bool laughPlayed = false;

int cpuHitX = -1, cpuHitY = -1;         // recent hit cords. of computer
bool targetFound = false;
int directions[4][2] = { {1, 0}, {-1, 0} , {0, -1}, {0, 1}  };        // Right, left, up, down
int currentDirection = 0;       // tells the direction being tested


// Function signaturess
void mainMenu();
void drawLeaderboard();
void fleetPlacement();
void Battle();
void GameOver();
void drawGrids(int grid[grid_length][grid_length], int initial_x, int initial_y, bool hideShips, int previewX, int previewY, int previewSize, bool previewHorizontal, int cellsize);
void PlaceComputerFleet();
void PlayerShoot(int x, int y, bool&);
void ComputerShoot();
int validPlacement(int grid[grid_length][grid_length], int x, int y, int size, bool horizontal);
void PlaceShip(int grid[grid_length][grid_length], int x, int y, int size, bool horizontal);
int RemainingShips(int grid[grid_length][grid_length]);
void updateLeaderboard(const string filename, const int score);
string enterNameWindow();
void roundedCornerRect(int posX, int posY, int width, int height, float radius, Color color);

//-----------------------------------------------------------------------------------------------------------------------------------------

Music bg_music;
Sound click;
Sound laugh;
Sound yay;

Texture2D bg;
Texture2D bgHover;
Texture2D bg_placement;
Texture2D bg_battle;
Texture2D leaderboard_btn;
Texture2D leaderboard_btn_hover;
Texture2D cross;
Texture2D cross_hover;
Texture2D gameover;
Texture2D home_btn;
Texture2D home_btn_hover;

//-----------------------------------------------------------------------------------------------------------------------------------------

void LoadTextures_and_Audios() {
    bg_music = LoadMusicStream("resources/sounds/bg.mp3");
    click = LoadSound("resources/sounds/click.mp3");
    laugh = LoadSound("resources/sounds/laugh.mp3");
    yay = LoadSound("resources/sounds/yay.mp3");

    bg = LoadTexture("resources/bg.png");
    bgHover = LoadTexture("resources/bg_hover.jpg");
    bg_placement = LoadTexture("resources/placement_bg.jpg");
    bg_battle = LoadTexture("resources/battle_bg.png");
    leaderboard_btn = LoadTexture("resources/leaderboard.png");
    leaderboard_btn_hover = LoadTexture("resources/leaderboard_hover.png");
    cross = LoadTexture("resources/cross.png");
    cross_hover = LoadTexture("resources/cross_hover.png");
    gameover = LoadTexture("resources/gameover.jpeg");
    home_btn = LoadTexture("resources/home_btn.png");
    home_btn_hover = LoadTexture("resources/home_btn_hover.png");

}

void UnloadTextures_and_Audios() {
    UnloadMusicStream(bg_music);

    UnloadTexture(bg);
    UnloadTexture(bgHover);
    UnloadTexture(bg_placement);
    UnloadTexture(bg_battle);
}



int main() {
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Battleship Game");
    SetTargetFPS(120);
    srand(time(NULL)); // for random no.

    InitAudioDevice();

    LoadTextures_and_Audios();

    PlayMusicStream(bg_music);

    PlaceComputerFleet();

    while (!WindowShouldClose()) {
        UpdateMusicStream(bg_music);
        if (screen == 0) {
            mainMenu();
        }
        else if (screen == 1) {
            fleetPlacement();
        }
        else if (screen == 2) {
            Battle();
        }
        else if (screen == 3) {
            GameOver();
        }
        else if (screen == 4) {
            drawLeaderboard();
        }
        else if (screen == 5) {
            enterNameWindow();
        }
    }

    CloseAudioDevice();
    UnloadTextures_and_Audios();
    CloseWindow();
    return 0;
}


void mainMenu() {

    // circular play button cords.
    float play_btnX = 975; // btn centerX
    float play_btnY = 690; // btn centerY
    float play_btn_radius = 200;
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    BeginDrawing();

    // checks if the mouse is in the area of play button
    float dx = mouseX - play_btnX + 20;
    float dy = mouseY - play_btnY + 22;
    bool isColliding = (dx * dx + dy * dy <= play_btn_radius * play_btn_radius);    //eq. of circle : x^2 + y^2 = r^2

    // lits play btn on hover
    if (isColliding) {

        DrawTexture(bgHover, -65, -40, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(click);
            screen = 5;       // enter name menu appears
        }
    }
    else {
        DrawTexture(bg, -65, -40, WHITE);   // Normal bg when not hovering
    }

    DrawTexture(leaderboard_btn, 80, 80, WHITE);
    if (mouseX >= 80 && mouseX <= 200 && mouseY >= 50 && mouseY <= 170) {
        DrawTexture(leaderboard_btn_hover, 80, 80, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            temp_screen = screen;
            PlaySound(click);
            screen = 4;         // leaderboard menu
        }
    }

    EndDrawing();

}



void drawLeaderboard() {
    int screenX = 880;      //lenth
    int screenY = 555;
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    int initial_x_cord = GetScreenWidth() / 2 - screenX / 2;        // starting cords of leaderboard window to center the window
    int initial_y_cord = GetScreenHeight() / 2 - screenY / 2;

    // cross button (on ldrbord) cordinates
    float crossbtn_x = initial_x_cord + screenX + 30; // btn centerX
    float crossbtn_y = initial_y_cord - 30; // btn centerY
    float crossbtn_radius = 30;

    // checks if the mouse is on of cross button
    float dx = mouseX - crossbtn_x + 20;
    float dy = mouseY - crossbtn_y + 22;
    bool isColliding = (dx * dx + dy * dy <= crossbtn_radius * crossbtn_radius);

    Color translucent = ColorAlpha(WHITE, 0.55);

    BeginDrawing();

    // for making backscreen where ldrbord is placed on according to the screen we pressed ldrbord btn on
    if (temp_screen == 0) {
        DrawTexture(bg, -65, -40, WHITE);
        DrawTexture(leaderboard_btn_hover, 80, 80, WHITE);
    }
    else if (temp_screen == 1) {
        DrawTexture(bg_placement, 0, 0, WHITE);
        DrawTexture(leaderboard_btn_hover, 110, 110, WHITE);
    }
    else if (temp_screen == 2) {
        DrawTexture(bg_battle, -20, 0, WHITE);
        DrawTexture(leaderboard_btn_hover, GetScreenWidth() / 2 - 70, GetScreenHeight() - 205, WHITE);
    }

    roundedCornerRect(initial_x_cord, initial_y_cord, screenX, screenY, 25, translucent);
    DrawTexture(cross, initial_x_cord + screenX - 30, initial_y_cord - 30, WHITE);


    //file handling 
    ifstream fr("leaderboard.txt");
    if (!fr) {
        DrawText("File not found", initial_x_cord, initial_y_cord + 120, 40, BLUE);
    }

    for (int i = 0; i < 3; i++) {           // reads names from txt file
        getline(fr, names[i]);
    }
    for (int i = 0; i < 3; i++) {           // reads scores from txt file
        fr >> scores[i];
    }

    DrawText("Leaderboard", initial_x_cord + screenX / 2 - MeasureText("Leaderboard", 60) / 2, initial_y_cord + 30, 60, BLACK);

    DrawText("Name", initial_x_cord + screenX / 4 - MeasureText("Name", 50) / 2, initial_y_cord + 130, 50, DARKBLUE);
    DrawText("Score", initial_x_cord + 3 * screenX / 4 - MeasureText("Score", 50) / 2, initial_y_cord + 130, 50, DARKBLUE);

    DrawText(names[0].c_str(), initial_x_cord + screenX / 4 - MeasureText(names[0].c_str(), 37) / 2, initial_y_cord + 225, 37, BLUE);
    DrawText(TextFormat("%d", scores[0]), initial_x_cord + 3 * screenX / 4 - MeasureText("88", 37) / 2, initial_y_cord + 225, 37, BLUE);

    DrawText(names[1].c_str(), initial_x_cord + screenX / 4 - MeasureText(names[1].c_str(), 37) / 2, initial_y_cord + +325, 37, BLUE);
    DrawText(TextFormat("%d", scores[1]), initial_x_cord + 3 * screenX / 4 - MeasureText("88", 37) / 2, initial_y_cord + 325, 37, BLUE);

    DrawText(names[2].c_str(), initial_x_cord + screenX / 4 - MeasureText(names[2].c_str(), 37) / 2, initial_y_cord + 425, 37, BLUE);
    DrawText(TextFormat("%d", scores[2]), initial_x_cord + 3 * screenX / 4 - MeasureText("88", 37) / 2, initial_y_cord + 425, 37, BLUE);

    if (isColliding) {
        DrawTexture(cross_hover, initial_x_cord + screenX - 30, initial_y_cord - 30, WHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            screen = temp_screen;
            PlaySound(click);
        }
    }

    // additional feature 
    if (IsKeyPressed(KEY_X)) {
        screen = temp_screen;
        PlaySound(click);
    }
    EndDrawing();
}


string enterNameWindow() {
    int screenX = 800;
    int screenY = 250;
    int initial_x_cord = GetScreenWidth() / 2 - screenX / 2 + 15;
    int initial_y_cord = GetScreenHeight() / 2 - screenY / 2;
    int maxLength = 25;
    int letterCount = 0;
    static int count_frame = 0;    // for blinking cursor

    

    while (letterCount > 0 || !IsKeyPressed(KEY_ENTER)) {
        UpdateMusicStream(bg_music);
        BeginDrawing();

        // Makes translucent background for the name input window
        Color translucent = ColorAlpha(WHITE, 0.57);
        DrawTexture(bg, -65, -40, WHITE);
        DrawTexture(leaderboard_btn_hover, 80, 80, WHITE);          // shows the leaderboard btn in pressed state in bg

        roundedCornerRect(initial_x_cord, initial_y_cord, screenX, screenY, 15, translucent);   //cretaes input window


        DrawText("Enter your name : ", initial_x_cord + 30, initial_y_cord + 30, 35, DARKGRAY);


        DrawText(name.c_str(), initial_x_cord + 30, initial_y_cord + 120, 35, BLUE);

        // cursor cords
        int cursorX = initial_x_cord + 32 + MeasureText(name.c_str(), 35);
        int cursorY = initial_y_cord + 120;


        // blinks cursor
        if ((count_frame / 30) % 2 == 0) {
            DrawText("|", cursorX, cursorY, 35, BLUE);
        }

        int key = GetKeyPressed();      // getkeypressed() func. returns the ascii and saves in 'key'


        while (key > 0) {
            if (key >= 32 && key <= 125 && letterCount < maxLength) {
                name += static_cast<char>(key); // adds the character to the name
                letterCount++;
            }
            key = GetKeyPressed();  // Check next key press
        }

        if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
            name.pop_back(); // Remove last character entered by user
            letterCount--;
        }

        // Check if ENTER is pressed and move to screen 1
        if (IsKeyPressed(KEY_ENTER) && letterCount > 0 && name != " " && name != "  " && name != "  " && name != "   ") {
            screen = 1;
            PlaySound(click);
            break;
        }

        EndDrawing();

        count_frame++;
    }

    return name;
}





void fleetPlacement() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    // initial cords. of grid
    int gridX = ( (screenW - (grid_length * fleet_placement_cell_size) ) / 2) + 42;         // (1200 - (10 * 60)) / 2 + 42
    int gridY = 290;

    BeginDrawing();

    DrawTexture(bg_placement, 0, 0, WHITE);

    // leadboard btn, hover and press
    DrawTexture(leaderboard_btn, 110, 110, WHITE);
    if (mouseX >= 110 && mouseX <= 230 && mouseY >= 80 && mouseY <= 200) {
        DrawTexture(leaderboard_btn_hover, 110, 110, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            temp_screen = screen;
            PlaySound(click);
            screen = 4;
        }
    }


    if (currentfleetSize == 0) {
        DrawText("Press SPACE to continue.", screenW / 2 - MeasureText("Left-click to place ships. Press SPACE to continue.", 34) / 2 + 280, 180, 31, WHITE);
    }
    else {
        DrawText("Press 'R' to rotate the ship direction.", screenW / 2 - MeasureText("Press 'R' to rotate the ship direction.", 34) / 2 + 66, 180, 31, WHITE);
    }

    DrawText(TextFormat("Ship Size: %d", currentfleetSize), 100, 400, 45, WHITE);


    drawGrids(playerGrid, gridX, gridY, false, -1, -1, 0, true, fleet_placement_cell_size);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

        int gridCellX = (mouseX - gridX) / fleet_placement_cell_size;
        int gridCellY = (mouseY - gridY + 30) / fleet_placement_cell_size;


        // Ensure mouse is within the grid's boundary
        if (gridCellX >= 0 && gridCellX < grid_length && gridCellY >= 0 && gridCellY < grid_length) {

            // now see if the placement is valid
            if (shipsPlaced < 5 && validPlacement(playerGrid, gridCellX, gridCellY, currentfleetSize, horizontal)) {
                PlaySound(click);
                PlaceShip(playerGrid, gridCellX, gridCellY, currentfleetSize, horizontal);
                shipsPlaced++;

                // places nxt fleet 
                if (shipsPlaced < 5) {
                    currentfleetSize = playerFleetSizes[shipsPlaced];
                }
                else {
                    currentfleetSize = 0;
                }
            }
        }
    }

    // Rotate the ship
    if (IsKeyPressed(KEY_R)) {
        horizontal = !horizontal;
        PlaySound(click);
    }

    // preview ships on grid with mouse hover
    int previewGridX = -1, previewGridY = -1;

    previewGridX = (mouseX - gridX) / fleet_placement_cell_size;        // row no.
    previewGridY = (mouseY - gridY + 30) / fleet_placement_cell_size;   // column no.

    // see if mouse is within grid to give a preview
    if (previewGridX >= 0 && previewGridX < grid_length && previewGridY >= 0 && previewGridY < grid_length) {

        // Draws the grid to preview ship placement w the correct cell size 
        drawGrids(playerGrid, gridX, gridY, false, previewGridX, previewGridY, currentfleetSize, horizontal, fleet_placement_cell_size);
    }


    if (shipsPlaced >= 5 && IsKeyPressed(KEY_SPACE)) {
        screen = 2;
        PlaySound(click);
    }

    EndDrawing();
}



void Battle() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    BeginDrawing();

    DrawTexture(bg_battle, -20, 0, WHITE);

    DrawTexture(leaderboard_btn, GetScreenWidth() / 2 - 70, GetScreenHeight() - 205, WHITE);
    if (mouseX >= GetScreenWidth() / 2 - 70 && mouseX <= GetScreenWidth() / 2 + 50 && mouseY >= GetScreenHeight() - 205 && mouseY <= GetScreenHeight() - 85) {
        DrawTexture(leaderboard_btn_hover, GetScreenWidth() / 2 - 70, GetScreenHeight() - 205, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            temp_screen = screen;
            PlaySound(click);
            screen = 4;
        }
    }


    drawGrids(playerGrid, 263, 257, false, -1, -1, 0, true, Battle_mode_cell_size);


    drawGrids(computerGrid, 1227, 257, true, -1, -1, 0, true, Battle_mode_cell_size);


    DrawText(TextFormat("Score: %d", playerScore), GetScreenWidth() / 2 - 120, 75, 47, WHITE);


    int playerShipParts = RemainingShips(playerGrid);
    int computerShipParts = RemainingShips(computerGrid);
    bool status = true;
    DrawText(TextFormat("Player Ship Parts Remaining : %d/17", playerShipParts), 168, screenH - 123, 36, WHITE);
    DrawText(TextFormat("Computer Ship Parts Remaining : %d/17", computerShipParts), screenW - 800, screenH - 123, 36, WHITE);

    // playeer shooting
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

        int gridX = (mouseX - 1227) / Battle_mode_cell_size;
        int gridY = (mouseY - 228) / Battle_mode_cell_size;


        // checks if mouse is within the grid 
        if (gridX >= 0 && gridX < grid_length && gridY >= 0 && gridY < grid_length && computerGrid[gridX][gridY] != 2 && computerGrid[gridX][gridY] != 3) {

            PlayerShoot(gridX, gridY, status);
            ComputerShoot();
            if (status == false) {
                playerScore--;
            }

        }
    }


    if (RemainingShips(playerGrid) == 0 || RemainingShips(computerGrid) == 0) {
        temp_screen = screen;
        screen = 3;
    }
    EndDrawing();
}




void GameOver() {
    BeginDrawing();
    int playerShipParts = RemainingShips(playerGrid);
    int computerShipParts = RemainingShips(computerGrid);
    int count = 0;
    int initial_x_cord = 347;
    int initial_y_cord = 380;
    int screenX = 1220;
    int screenY = 600;
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    Color translucent = ColorAlpha(WHITE, 0.3);       // translucent leaderboard bg color


    DrawTexture(gameover, -90, 30, WHITE);      // draws bg pic.


    // this is done to center the name and status of player
    int name_length = MeasureText(name.c_str(), 72);
    int temp_length = MeasureText(" WON!", 72);

    // text on game over screen 
    if (playerShipParts == 0) {
        DrawText(TextFormat("%s LOST!", name.c_str()), initial_x_cord + screenX / 2 - (name_length + temp_length) / 2, initial_y_cord - 80, 72, RED);

        if (!laughPlayed) {
            SetSoundVolume(laugh, 3);
            PlaySound(laugh);
            laughPlayed = true;
        }
        won = false;
    }
    else if (computerShipParts == 0) {
        DrawText(TextFormat("%s WON!", name.c_str()), initial_x_cord + screenX / 2 - (name_length + temp_length) / 2, initial_y_cord - 80, 72, GREEN);
        
        if (!yayPlayed) {
            PlaySound(yay);
            yayPlayed = true;
        }
        updateLeaderboard("leaderboard.txt", playerScore);
    }


    DrawText(TextFormat("Score: %d", playerScore), initial_x_cord - 75, initial_y_cord - 160, 43, WHITE);


    // home btn (on top left) to reset everythng
    DrawTexture(home_btn, 35, 70, WHITE);
    if (mouseX >= 30 && mouseX <= 130 && mouseY >= 30 && mouseY <= 130) {
        DrawTexture(home_btn_hover, 35, 70, WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(click);
            screen = 0;
            playerScore = 100;
            name = "";
            shipsPlaced = 0;
            currentfleetSize = 5;

            // Resets the game
            for (int i = 0; i < grid_length; i++) {
                for (int j = 0; j < grid_length; j++) {
                    playerGrid[i][j] = 0;
                    computerGrid[i][j] = 0;
                }
            }

            PlaceComputerFleet();
            horizontal = !horizontal;       // so if player plays the game again, the fleets in fleet placement screen are by default horizonatal

            yayPlayed = false;
            laughPlayed = false;
        }
    }


    //leaderboard on gameover screen
    roundedCornerRect(initial_x_cord, initial_y_cord, screenX, screenY, 30, translucent);


    DrawText("Name", initial_x_cord + screenX / 4 - MeasureText("Name", 50) / 2, initial_y_cord + 130, 50, BLACK);  // Title
    DrawText("Score", initial_x_cord + 3 * screenX / 4 - MeasureText("Score", 50) / 2, initial_y_cord + 130, 50, BLACK);  // Title

    DrawText(names[0].c_str(), initial_x_cord + screenX / 4 - MeasureText(names[0].c_str(), 37) / 2, initial_y_cord + 225, 37, BLUE);    // Example entry
    DrawText(TextFormat("%d", scores[0]), initial_x_cord + 3 * screenX / 4 - MeasureText("88", 37) / 2, initial_y_cord + 225, 37, BLUE);   // Example entry

    DrawText(names[1].c_str(), initial_x_cord + screenX / 4 - MeasureText(names[1].c_str(), 37) / 2, initial_y_cord + +325, 37, BLUE);    // Example entry
    DrawText(TextFormat("%d", scores[1]), initial_x_cord + 3 * screenX / 4 - MeasureText("88", 37) / 2, initial_y_cord + 325, 37, BLUE);    // Example entry

    DrawText(names[2].c_str(), initial_x_cord + screenX / 4 - MeasureText(names[2].c_str(), 37) / 2, initial_y_cord + 425, 37, BLUE);    // Example entry
    DrawText(TextFormat("%d", scores[2]), initial_x_cord + 3 * screenX / 4 - MeasureText("88", 37) / 2, initial_y_cord + 425, 37, BLUE);    // Example entry



    EndDrawing();
}



void roundedCornerRect(int x_cord, int y_cord, int width, int height, float radius, Color color) {

    // Draw the central rectangles
    DrawRectangle(x_cord + radius, y_cord, width - 2 * radius, height, color);
    DrawRectangle(x_cord, y_cord + radius, width, height - 2 * radius, color);

    // Draw circles on corners
    DrawCircle(x_cord + radius, y_cord + radius, radius, color);
    DrawCircle(x_cord + width - radius, y_cord + radius, radius, color);
    DrawCircle(x_cord + radius, y_cord + height - radius, radius, color);
    DrawCircle(x_cord + width - radius, y_cord + height - radius, radius, color);
}


void updateLeaderboard(const string filename, const int score) {        // this just updates the ldrbord on gameoevr screen

    ifstream fr(filename);

    // Storing data in name and score arrays
    for (int i = 0; i < 3; i++) {
        fr >> names[i];
    }
    for (int i = 0; i < 3; i++) {
        fr >> scores[i];
    }
    fr.close();

    if (won == false) {

    }
    else {
        // Insert the new score into the correct order... 1st 2nd 3rd
        if (score > scores[0]) {
            scores[2] = scores[1];
            scores[1] = scores[0];
            scores[0] = score;

            names[2] = names[1];
            names[1] = names[0];
            names[0] = name;
        }
        else if (score > scores[1] && score < scores[0]) {
            scores[2] = scores[1];
            scores[1] = score;

            names[2] = names[1];
            names[1] = name;
        }
        else if (score > scores[2] && score < scores[1]) {
            scores[2] = score;

            names[2] = name;
        }
    }



    // Emptying the file
    ofstream empty(filename);
    empty << "";
    empty.close();


    // Write the updated leaderboard to the file in append mode
    ofstream fw_leaderboard(filename, ios::app);

    for (int i = 0; i < 3; i++) {
        fw_leaderboard << names[i] << endl;
    }
    for (int i = 0; i < 3; i++) {
        fw_leaderboard << scores[i] << endl;
    }
    fw_leaderboard.close();

}



void drawGrids(int grid[grid_length][grid_length], int initial_x, int initial_y, bool hideShips, int previewX, int previewY, int previewSize, bool previewHorizontal, int cellsize) {

    float opacity = 0.6;

    Texture2D hit_img = LoadTexture("resources/hit.png");
    Texture2D miss_img = LoadTexture("resources/miss.png");
    Texture2D ship_img = LoadTexture("resources/ship.png");
    Texture2D preview_ship_img = LoadTexture("resources/preview_ship.png");

    // ek ek cell draw krta hai according which makes up a grid...har click pr kese ek cell ki from each grid state change hoti
    for (int i = 0; i < grid_length; i++) {         // i for row no
        for (int j = 0; j < grid_length; j++) {     // j for column no
            int x = initial_x + i * cellsize;       // initial cord of each cell
            int y = initial_y + j * cellsize;


            Color wheat = { 255, 251, 229 };
            Color cellColor = ColorAlpha(wheat, opacity);

            // Changing colors based on grid value
            if (!hideShips && grid[i][j] == 1) {
                cellColor = ColorAlpha(WHITE, opacity); // Ship
            }
            else if (grid[i][j] == 2) {
                cellColor = ColorAlpha(WHITE, opacity);              // Hit
            }
            else if (grid[i][j] == 3) {
                cellColor = ColorAlpha(WHITE, opacity);             // Miss
            }


            DrawRectangle(x, y, cellsize, cellsize, cellColor);     // draws a single cell
            DrawRectangleLines(x, y, cellsize, cellsize, BLACK);


            // adding textures (mini bg) to cells
            if (!hideShips && grid[i][j] == 1) {
                if (cellsize == fleet_placement_cell_size) {
                    DrawTexture(preview_ship_img, x, y, WHITE);
                }
                else {
                    DrawTexture(ship_img, x, y, WHITE);
                }

            }
            else if (grid[i][j] == 2) {
                DrawTexture(hit_img, x, y, WHITE);
            }
            else if (grid[i][j] == 3) {
                DrawTexture(miss_img, x, y, WHITE);
            }

        }
    }


    // ship placemeent ka preview
    if (previewSize > 0) {
        for (int i = 0; i < previewSize; i++) {
            int x, y;

            if (previewHorizontal) {
                x = previewX + i;
                y = previewY;
            }
            else {
                x = previewX;
                y = previewY + i;
            }

            if (x < 0 || x >= grid_length || y < 0 || y >= grid_length) {       //moves preview outta screen
                continue;
            }
                

            int screenX = initial_x + x * cellsize;
            int screenY = initial_y + y * cellsize;

            DrawTexture(preview_ship_img, screenX, screenY, WHITE);
        }
    }

}

void PlaceComputerFleet() {
    for (int i = 0; i < 5; i++) {
        int x, y;
        bool horizontal = rand() % 2;
        bool placed = false;

        while (!placed) {
            x = rand() % grid_length;
            y = rand() % grid_length;
            if (validPlacement(computerGrid, x, y, computerFleetSizes[i], horizontal)) {
                PlaceShip(computerGrid, x, y, computerFleetSizes[i], horizontal);
                placed = true;
            }
        }
    }
}

int validPlacement(int grid[grid_length][grid_length], int x, int y, int size, bool horizontal) {
    if (horizontal) {
        if (x + size > grid_length) {       // for boundary check
            return 0;
        }
        for (int i = 0; i < size; i++) {  // for overlap chcek
            if (grid[x + i][y] != 0) {
                return 0;
            }
        }
    }
    else {
        if (y + size > grid_length) {       // for boundary check
            return 0;
        }
        for (int i = 0; i < size; i++) {  // for overlap chcek
            if (grid[x][y + i] != 0) {
                return 0;
            }
        }
    }
    return 1;
}

void PlaceShip(int grid[grid_length][grid_length], int x, int y, int currentFLeetSize, bool horizontal) {
    for (int i = 0; i < currentFLeetSize; i++) {
        if (horizontal) {
            grid[x + i][y] = 1;
        } 
        else {
            grid[x][y + i] = 1;
        }
 
    }
}

void PlayerShoot(int x, int y, bool& status) {
    Sound heheBoy = LoadSound("resources/sounds/hehe boy.mp3");
    Sound miss = LoadSound("resources/sounds/player_shoot.mp3");

    if (computerGrid[x][y] == 1) {
        SetSoundVolume(heheBoy, 6.5);
        PlaySound(heheBoy);
        computerGrid[x][y] = 2; // Hit
        computerShipsRemaining--;
    }
    else {
        PlaySound(miss);
        status = false;
        computerGrid[x][y] = 3; // Miss
    }
}

void ComputerShoot() {
    int x, y;
    Sound shoot = LoadSound("resources/sounds/cpu_shoot.mp3");

    if (targetFound) {

        // seraches around (left, right, up down) the point where it hit previously
        while (currentDirection < 4) {     // 4 bcs cpu need to check four sides
            int initial_x = directions[currentDirection][0];        // adds the row part of direction array into x cord
            int initial_y = directions[currentDirection][1];        // adds the column part of direction array into y cord

            x = cpuHitX + initial_x;
            y = cpuHitY + initial_y;
            if (x >= 0 && x < grid_length && y >= 0 && y < grid_length && playerGrid[x][y] != 2 && playerGrid[x][y] != 3) {
                break;
            }

            currentDirection++;
        }

        // when cpu doesnot finds another part of ship on 4 sides, it resets and starts shooting randomly again
        if (currentDirection >= 4) {
            targetFound = false;
            currentDirection = 0;
        }
    }

    if (!targetFound) {
        // Random shooting 
        do {
            x = (rand() + rand() * rand()) % grid_length;
            y = (rand() + rand() * rand()) % grid_length;
        } 
        while (playerGrid[x][y] == 2 || playerGrid[x][y] == 3); // this doesnot shoots the previously hit cells
    }


    if (playerGrid[x][y] == 1) {
        playerGrid[x][y] = 2; // changes the state of cell when it is hit
        playerShipsRemaining--;
        cpuHitX = x;
        cpuHitY = y;
        targetFound = true;
        currentDirection = 0;
    }
    else {

        playerGrid[x][y] = 3; // cpu misses the target
    }
}


int RemainingShips(int grid[grid_length][grid_length]) {
    int count = 0;
    for (int i = 0; i < grid_length; i++) {
        for (int j = 0; j < grid_length; j++) {
            if (grid[i][j] == 1) count++;
        }
    }
    return count;
}


// MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW MEOW // 