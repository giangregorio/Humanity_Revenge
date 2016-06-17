#define SHOOT_ONLY     1
#define ATTACK_FIST    2
// #define MOVE_AHEAD     2

class Boss
{
  private:
    byte x = 0;
    byte y = 0;

    unsigned long attackchange = 0;
    unsigned long dead = 0;
  public:
    // 2 attacktype, 1 goingup, 1alternate engine, 1 visible
    byte bossstatus = 0b00000000;

    void spawnboss(Enemy enemies[])
    {
      x = 90;
      y = 20;
      bossstatus = 0b01111000;
      attackchange = millis();

      enemies[0].enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      enemies[0].x = x;
      enemies[0].y = y;
      enemies[0].hitboxXPadding = 0;
      enemies[0].hitboxYPadding = 0;
      enemies[0].hitboxWidth = 16;
      enemies[0].hitboxHeight = 16;
      enemies[0].enemylife = 60;
      
      enemies[1].x = x;
      enemies[1].y = y + 16;
      enemies[1].enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      enemies[1].hitboxXPadding = 0;
      enemies[1].hitboxYPadding = 0;
      enemies[1].hitboxWidth = 8;
      enemies[1].hitboxHeight = 8;
      enemies[1].enemylife = 25;

      enemies[2].x = x;
      enemies[2].y = y - 8;
      enemies[2].enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      enemies[2].hitboxXPadding = 0;
      enemies[2].hitboxYPadding = 0;
      enemies[2].hitboxWidth = 8;
      enemies[2].hitboxHeight = 8;
      enemies[2].enemylife = 25;

      enemies[3].x = x + 8;
      enemies[3].y = y + 16;
      enemies[3].enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      enemies[3].hitboxXPadding = 0;
      enemies[3].hitboxYPadding = 0;
      enemies[3].hitboxWidth = 8;
      enemies[3].hitboxHeight = 8;
      enemies[3].enemylife = 10;
    
      enemies[4].x = x + 8;
      enemies[4].y = y - 8;
      enemies[4].enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      enemies[4].hitboxXPadding = 0;
      enemies[4].hitboxYPadding = 0;
      enemies[4].hitboxWidth = 8;
      enemies[4].hitboxHeight = 8;
      enemies[4].enemylife = 10;
    }

    bool isdead(Enemy enemies[])
    {
      return (dead != 0 && (millis() - dead >= 2000));
    }

    void draw(Enemy enemies[])
    {
      if ((bossstatus & 0b00001000) == 0b00001000)
      {
        if (arduboy.everyXFrames(15))
          bossstatus ^= 0b00010000;

        if ((enemies[0].enemystatus & 0b00001000) == 0b00001000)
        {
          if ((bossstatus & 0b00010000) == 0b00010000)
            arduboy.drawSlowXYBitmap(x, y, mainpart1, 16, 16, 1);
          else
            arduboy.drawSlowXYBitmap(x, y, mainpart2, 16, 16, 1);
        }
        if ((enemies[1].enemystatus & 0b00001000) == 0b00001000)
          arduboy.drawSlowXYBitmap(enemies[1].x, y + 16, bottomaheadpart, 8, 8, 1);
        if ((enemies[2].enemystatus & 0b00001000) == 0b00001000)
          arduboy.drawSlowXYBitmap(enemies[2].x, y - 8, upperaheadpart, 8, 8, 1);
        if ((enemies[3].enemystatus & 0b00001000) == 0b00001000)
          arduboy.drawSlowXYBitmap(x + 8, y + 16, bottomretropart, 8, 8, 1);
        if ((enemies[4].enemystatus & 0b00001000) == 0b00001000)
          arduboy.drawSlowXYBitmap(x + 8, y - 8, upperretropart, 8, 8, 1);
      }
    }

    void update(Enemy enemies[])
    {
      if ((bossstatus & 0b00001000) == 0b00001000)
      {
        if (dead == 0 && !((enemies[0].enemystatus & 0b00001000) == 0b00001000))
          dead = millis();

        if (millis() - attackchange > 5000)
        {
          // reset going up e set new attacktype!
          bossstatus &= 0b00111111;
          bossstatus ^= 0b00100000;
          bossstatus |= (byte)(random(SHOOT_ONLY, 3) << 6);
          attackchange = millis();
        }

        if ((bossstatus >> 6) == SHOOT_ONLY)
        {
          if (x < 90)
            x++;

          if ((bossstatus & 0b00100000) == 0b00100000)
          {
            y--;
            if (y <= MIN_Y + 8 + 3)
              bossstatus ^= 0b00100000;
          } else
          {
            y++;
            if (y > 37)
              bossstatus ^= 0b00100000;
          }

          enemies[0].x = x;
          enemies[1].x = min(x, enemies[1].x + 3);
          enemies[2].x = min(x, enemies[2].x + 3);
          enemies[3].x = x + 8;
          enemies[4].x = x + 8;

          enemies[0].y = y;
          enemies[1].y = y + 16;
          enemies[2].y = y - 8;
          enemies[3].y = y + 16;
          enemies[4].y = y - 8;
        } else if ((bossstatus >> 6) == ATTACK_FIST)
        {
          if (x < 90)
            x = min(x + 3, 90);

          if ( (enemies[1].enemystatus & 0b00001000) == 0b00001000 || (enemies[2].enemystatus & 0b00001000) == 0b00001000 )
          {
            if ((bossstatus & 0b00100000) == 0b00100000)
            {
              enemies[1].x -= 2;
              enemies[2].x -= 2;
            }
            else
            {
              enemies[1].x = min(90, max(enemies[1].x + 1, 6));
              enemies[2].x = min(90, max(enemies[2].x + 1, 6));
            }

            if (enemies[1].x <= 5 || enemies[1].x >= 90)
              bossstatus ^= 0b00100000;
          }

          enemies[0].x = x;
          enemies[3].x = x + 8;
          enemies[4].x = x + 8;

          enemies[0].y = y;
          enemies[1].y = y + 16;
          enemies[2].y = y - 8;
          enemies[3].y = y + 16;
          enemies[4].y = y - 8;
        }
      }
    }
};

