#include "eduterm.h"
#include "raylib.h"

// Check if any key is pressed
// NOTE: We limit keys check to keys between 32 (KEY_SPACE) and 126
bool IsAnyKeyPressed() {
  bool keyPressed = false;
  int key = GetKeyPressed();

  if ((key >= 32) && (key <= 126))
    keyPressed = true;

  return keyPressed;
}

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

#define MAX_INPUT_CHARS 99
#define MAX_FRAMES 3600

char name[MAX_INPUT_CHARS + 1] =
    "\0"; // NOTE: One extra space required for null terminator char '\0'
int letterCount = 0;

Rectangle textBox = {0, 0, screenWidth, screenHeight};
bool mouseOnText = false;

int framesCounter = 0;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void); // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void) {
  // eduterm_main();
  // Initialization
  //---------------------------------------------------------
  InitWindow(screenWidth, screenHeight, "raylib game template");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateDrawFrame();
    if (framesCounter > MAX_FRAMES) {
      framesCounter = 0;
    }
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  // Unload current screen data before closing

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
  // Update
  //----------------------------------------------------------------------------------
  // Set the window's cursor to the I-Beam
  SetMouseCursor(MOUSE_CURSOR_IBEAM);

  // Get char pressed (unicode character) on the queue
  int key = GetCharPressed();

  // Check if more characters have been pressed on the same frame
  while (key > 0) {
    // NOTE: Only allow keys in range [32..125]
    if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS)) {
      name[letterCount] = (char)key;
      name[letterCount + 1] =
          '\0'; // Add null terminator at the end of the string.
      letterCount++;
    }

    key = GetCharPressed(); // Check next character in the queue
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    letterCount--;
    if (letterCount < 0)
      letterCount = 0;
    name[letterCount] = '\0';
  }

  framesCounter++;

  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(BLACK);

  // Border
  DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width,
                     (int)textBox.height, RED);

  // Draw the actual text
  DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

  // Allow for backspace
  if (letterCount < MAX_INPUT_CHARS) {
    // Draw blinking underscore char
    if (((framesCounter / 20) % 2) == 0)
      DrawText("_", (int)textBox.x + 8 + MeasureText(name, 40),
               (int)textBox.y + 12, 40, MAROON);
  }

  EndDrawing();
  //----------------------------------------------------------------------------------
}
