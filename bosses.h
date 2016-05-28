#define SHOOT_ONLY     1
#define ATTACK_FIST    2
// #define MOVE_AHEAD     2

class Boss
{
  private:
    byte x;
    byte y;

    unsigned long attackchange;
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

      Enemy mainpart;
      mainpart.enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      mainpart.x = x;
      mainpart.y = y;
      mainpart.hitboxXPadding = 0;
      mainpart.hitboxYPadding = 0;
      mainpart.hitboxWidth = 16;
      mainpart.hitboxHeight = 16;
      mainpart.enemylife = 60;
      enemies[0] = mainpart;

      Enemy bottomaheadpart;
      bottomaheadpart.x = x;
      bottomaheadpart.y = y + 16;
      bottomaheadpart.enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      bottomaheadpart.hitboxXPadding = 0;
      bottomaheadpart.hitboxYPadding = 0;
      bottomaheadpart.hitboxWidth = 8;
      bottomaheadpart.hitboxHeight = 8;
      bottomaheadpart.enemylife = 25;
      enemies[1] = bottomaheadpart;

      Enemy upperaheadpart;
      upperaheadpart.x = x;
      upperaheadpart.y = y - 8;
      upperaheadpart.enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      upperaheadpart.hitboxXPadding = 0;
      upperaheadpart.hitboxYPadding = 0;
      upperaheadpart.hitboxWidth = 8;
      upperaheadpart.hitboxHeight = 8;
      upperaheadpart.enemylife = 25;
      enemies[2] = upperaheadpart;

      Enemy bottomretropart;
      bottomretropart.x = x + 8;
      bottomretropart.y = y + 16;
      bottomretropart.enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      bottomretropart.hitboxXPadding = 0;
      bottomretropart.hitboxYPadding = 0;
      bottomretropart.hitboxWidth = 8;
      bottomretropart.hitboxHeight = 8;
      bottomretropart.enemylife = 10;
      enemies[3] = bottomretropart;

      Enemy upperretropart;
      upperretropart.x = x + 8;
      upperretropart.y = y - 8;
      upperretropart.enemystatus = (byte)(BOSS_PART << 6) | 0b00001000;
      upperretropart.hitboxXPadding = 0;
      upperretropart.hitboxYPadding = 0;
      upperretropart.hitboxWidth = 8;
      upperretropart.hitboxHeight = 8;
      upperretropart.enemylife = 10;
      enemies[4] = upperretropart;
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

