#include "Arduboy.h"
#include "ArduboyExtra.h"
#include "simple_buttons.h"

#include "title.h"
#include "sprites.h"

Arduboy arduboy;
// AbPrinter text(arduboy);
SimpleButtons buttons (arduboy);

byte currentlevel = 1;
bool haveToChangeLevel = false;
bool changingLevel = false;
unsigned long levelTimer = 0;

#define MIN_Y                 7
#define MAX_Y                 64

word global_score = 0;

void UpdateGlobalScore(byte inc)
{
  global_score += inc;
}

#include "explosions.h"
#include "enemy.h"
#include "hitbox.h"
#include "ship.h"
#include "background.h"

bool initializeNewGame = true;
Ship ship;
EnemyManager enemym;
ExplosionManager explosionm;
Background background;

// game states
#define TITLE_SCREEN          0
#define GAMING                1
#define PAUSE                 2
#define GAME_OVER             3
#define VICTORY               4

byte gameState = TITLE_SCREEN;

void setup() {
  arduboy.beginNoLogo();
  arduboy.setFrameRate(60);

  arduboy.initRandomSeed();
}

// define our own nextFrame so we can inject other libraries that may
// need to poll or do things per frame
boolean nextFrame()
{
  if (arduboy.nextFrame()) {
    buttons.poll();
    return true;
  }
  return false;
}

void loop() {
  if (!(nextFrame()))
    return;

  arduboy.clear();

  switch (gameState)
  {
    case TITLE_SCREEN:
      DrawTitleScreen();

      if (buttons.pressed(A_BUTTON) || buttons.pressed(B_BUTTON))
        gameState = GAMING;
      break;
    case GAMING:
      {
        if (initializeNewGame)
        {
          global_score = 0;
          levelTimer = millis();
          currentlevel = 1;
          haveToChangeLevel = false;
          changingLevel = true;

          ship.init();
          enemym.clr();
          explosionm.clr();
          background.clr();

          initializeNewGame = false;
        }

        if (changingLevel)
        {
          arduboy.setTextSize(1);
          arduboy.setCursor(3, 3);

          switch (currentlevel)
          {
            case 1:
              arduboy.println("1 - Counterattack");
              break;
            case 2:
              arduboy.println("2 - Reinforce");
              break;
            case 3:
              arduboy.println("3 - The Kamikaze");
              break;
            case 4:
              arduboy.println("4 - Getting Bigger");
              break;
            case 5:
              arduboy.println("5 - Keep shooting");
              break;
            case 6:
              arduboy.println("6 - Oh Oh");
              break;
          }

          if (millis() - levelTimer > 3000)
          {
            changingLevel = false;
            enemym.clr();
          }
        }

        if ((millis() - levelTimer > 30000 /*word(30000 + (currentlevel * 3000))*/) && currentlevel < 6 && !changingLevel && !haveToChangeLevel)
          haveToChangeLevel =  true;

        if (haveToChangeLevel)
        {
          if (!enemym.arevisibleenemies())
          {
            levelTimer = millis();
            haveToChangeLevel = false;
            changingLevel = true;
            currentlevel++;
          }
        }

        if (!changingLevel)
        {
          DrawStatusBar();
          enemym.update(ship.shipX, ship.shipY, explosionm);
        }

        background.update();
        ship.getinput();
        ship.update(enemym);

        ship.draw();
        explosionm.draw();
        background.draw();
        if (!changingLevel)
        {
          enemym.draw();
          ship.checkcollisions(enemym, explosionm);

          if ((ship.shipstatus >> 6 | 0b00000000) == 0b00000000)
          {
            gameState = GAME_OVER;
            delay(1000);
            return;
          }

          enemym.spawn(ship.shipX, ship.shipY);

          if (currentlevel == 6 && enemym.bossdead())
          {
            gameState = VICTORY;
            delay(1000);
            return;
          }
        }

        break;
      }
    case GAME_OVER:
    case VICTORY:
      {
        initializeNewGame = true;

        if (gameState == GAME_OVER)
          DrawGameOver();
        else if (gameState == VICTORY)
          DrawVictory();

        if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON))
        {
          gameState = TITLE_SCREEN;
          delay(200);
        }
        break;
      }
  }

  arduboy.display();
}

void DrawTitleScreen()
{
  arduboy.drawSlowXYBitmap(0, 0, title_screen, 128, 64, 1);
}

void DrawGameOver()
{
  arduboy.setCursor(8, 22);
  arduboy.setTextSize(2);
  arduboy.println("game over!");
  arduboy.setTextSize(1);
  arduboy.print("   score: ");
  arduboy.print(global_score);
}

void DrawVictory()
{
  arduboy.setCursor(8, 22);
  arduboy.setTextSize(2);
  arduboy.println("YOU WIN!");
  arduboy.setTextSize(1);
  arduboy.print("   score: ");
  arduboy.print(global_score);
}

void drawMiniShip(byte x)
{
  arduboy.drawFastHLine(x + 2, 0, 2, 1);
  arduboy.drawFastHLine(x, 1, 3, 1);
  arduboy.drawFastHLine(x, 2, 3, 1);
  arduboy.drawFastHLine(x + 2, 3, 2, 1);
}

void drawMiniBomb(byte x)
{
  arduboy.drawFastHLine(x + 1, 0, 2, 1);
  arduboy.drawFastHLine(x, 1, 4, 1);
  arduboy.drawFastHLine(x, 2, 4, 1);
  arduboy.drawFastHLine(x + 1, 3, 2, 1);
}

void DrawStatusBar()
{
  // draw lives
  if ((ship.shipstatus & 0b11000000) == 0b11000000)
  {
    drawMiniShip(1 * 6);
    drawMiniShip(2 * 6);
    drawMiniShip(3 * 6);
  }
  else if ((ship.shipstatus & 0b10000000) == 0b10000000)
  {
    drawMiniShip(1 * 6);
    drawMiniShip(2 * 6);
  }
  else if ((ship.shipstatus & 0b01000000) == 0b01000000)
    drawMiniShip(1 * 6);

  // draw bombs
  if ((ship.shipstatus & 0b00110000) == 0b00110000)
  {
    drawMiniBomb(22 + 1 * 6);
    drawMiniBomb(22 + 2 * 6);
    drawMiniBomb(22 + 3 * 6);
  }
  else if ((ship.shipstatus & 0b00100000) == 0b00100000)
  {
    drawMiniBomb(22 + 1 * 6);
    drawMiniBomb(22 + 2 * 6);
  }
  else if ((ship.shipstatus & 0b00010000) == 0b00010000)
    drawMiniBomb(22 + 1 * 6);

  arduboy.setCursor(50, -1);
  arduboy.setTextSize(1);
  arduboy.print("stage:");
  arduboy.setCursor(86, 0);
  arduboy.print(currentlevel);

  arduboy.setCursor(106, 0);
  arduboy.setTextSize(1);
  arduboy.print(global_score);
}


