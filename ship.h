#include "gun.h"
#include "gun_split.h"
#include "laser.h"
#include "bombs.h"

class Ship
{
  private:
    Gun gun;
    GunSplit gunsplit;
    Laser laser;
    Bomb bomb;

    unsigned long immunityStart = 0;
  public:
    int shipX, shipY;
    byte shipSpeed = 1;
    // 2 lives, 2 bombs, 2 weapons, 1 immune, 1 alternate
    byte shipstatus = 0b11110100;
    bool canchangeweapon = true;

    void init()
    {
      shipstatus = 0b11110100;
      immunityStart = 0;

      shipX = 0;
      shipY = 50 * 100;

      gun.clr();
      gunsplit.clr();
      laser.clr();
      bomb.clr();
    }

    void update(EnemyManager &enemym)
    {
      if ((shipstatus & 0b00000010) == 0b00000010)
        if (millis() - immunityStart > 3000)
        {
          shipstatus ^= 0b00000010;
          immunityStart = 0;
        }

      gun.update();
      gunsplit.update();
      laser.update();
      bomb.update(enemym);
    }

    void draw()
    {
      if (arduboy.everyXFrames(4))
        shipstatus ^= 0b00000001;

      if (!((shipstatus & 0b00000010) == 0b00000010) || arduboy.everyXFrames(5))
      {
#ifdef DRAWONLYHITBOX
        arduboy.drawRect((shipX / 100) + 2, (shipY / 100) + 2, 5 , 4, 1);
#else
        if (((shipstatus & 0b00000001) == 0b00000001))
          arduboy.drawBitmap(shipX / 100, shipY / 100, main_ship1, 8, 8, 1);
        else
          arduboy.drawBitmap(shipX / 100, shipY / 100, main_ship2, 8, 8, 1);
#endif
      }

      gun.draw();
      gunsplit.draw();
      laser.draw();
      bomb.draw();
    }

    void getinput()
    {
      if (buttons.pressed(LEFT_BUTTON))
      {
        if (shipX > 0)
        {
          shipX -= (shipSpeed * 100);
          if (shipX < 0)
            shipX = 0;
        }
      }

      if (buttons.pressed(RIGHT_BUTTON))
      {
        shipX += (shipSpeed * 100);
        if (shipX > 120 * 100)
          shipX = 120 * 100;
      }

      if (buttons.pressed(UP_BUTTON))
      {
        if (shipY > MIN_Y * 100)
        {
          shipY -= (shipSpeed * 100);
          if (shipY < MIN_Y * 100)
            shipY = MIN_Y * 100;
        }
      }

      if (buttons.pressed(DOWN_BUTTON))
      {
        shipY += (shipSpeed * 100);
        if (shipY > 56 * 100)
          shipY = 56 * 100;
      }

      if (buttons.pressed(A_BUTTON) && (buttons.justPressed(B_BUTTON)))
        shootbomb();
      else if (buttons.pressed(A_BUTTON))
        shoot();
      else if (buttons.justPressed(B_BUTTON))
        switch_weapon();
    }

    void shootbomb()
    {
      if (!bomb.visible)
        if (((byte)(shipstatus << 2) >> 6 ) >= 1)
        {
          shipstatus -= 0b00010000;

          bomb.visible = true;
          bomb.radius = 10;
          bomb.x = shipX / 100 + 3;
          bomb.y = shipY / 100 + 3;
        }
    }

    void shoot()
    {
      // laser
      if ((shipstatus & 0b00001100) == 0b00001100)
        laser.shoot((shipX / 100) + 8, shipY / 100);
      // gun
      else if ((shipstatus & 0b00000100) == 0b00000100)
        gun.shoot((shipX / 100) + 8, shipY / 100);
      // gun split
      else if ((shipstatus & 0b00001000) == 0b00001000)
        gunsplit.shoot((shipX / 100) + 8, shipY / 100);
    }

    void switch_weapon()
    {
      if (canchangeweapon)
      {
        // laser
        if ((shipstatus & 0b00001100) == 0b00001100)
          shipstatus ^= 0b00001000;
        // gun
        else if ((shipstatus & 0b00000100) == 0b00000100)
          shipstatus ^= 0b00001100;
        // gun split
        else if ((shipstatus & 0b00001000) == 0b00001000)
          shipstatus ^= 0b00000100;
      }
    }

    void decreaselife()
    {
      if (shipstatus >> 6 >= 1)
      {
        shipstatus -= 0b01000000;
        shipstatus ^= 0b00000010;
      }

      immunityStart = millis();
    }

    void checkcollisions(EnemyManager &enemym, ExplosionManager &explosionm)
    {
      gun.checkcollisions(enemym, explosionm);
      gunsplit.checkcollisions(enemym, explosionm);
      laser.checkcollisions(enemym, explosionm);
      bomb.checkcollisions(enemym, explosionm);

      if (!((shipstatus & 0b00000010) == 0b00000010))
      {
        for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
          if ((enemym.enemies[i].enemystatus & 0b00001000) == 0b00001000)
            if (checkhitbox((shipX / 100) + 2, (shipY / 100) + 2, 5 , 4, enemym.enemies[i].x + enemym.enemies[i].hitboxXPadding, enemym.enemies[i].y + enemym.enemies[i].hitboxYPadding, enemym.enemies[i].hitboxWidth, enemym.enemies[i].hitboxHeight))
            {
              byte damage = 3;
              enemym.damageenemy(enemym.enemies[i], damage, explosionm);
              decreaselife();
            }
      }

      if (!((shipstatus & 0b00000010) == 0b00000010))
      {
        for (int i = 0; i < MAX_ENEMY_SHOOTS; i++)
          if (enemym.shoots[i].visible)
            if (checkhitbox((shipX / 100) + 2, (shipY / 100) + 2, 5 , 4, enemym.shoots[i].x / 100, enemym.shoots[i].y / 100, 1, 1))
            {
              enemym.shoots[i].visible = false;
              decreaselife();
            }
      }
    }
};



