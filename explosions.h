#define MAX_EASY_ENEMY_ON_STAGE    7
#define MAX_MEDIUM_ENEMY_ON_STAGE  3
#define MAX_ENEMY_ON_STAGE         MAX_EASY_ENEMY_ON_STAGE + MAX_MEDIUM_ENEMY_ON_STAGE

struct Explosion {
  byte x = 0;
  byte y = 0;
  // 3maxstep, 1 visible, 4currentstep
  byte explosionstatus = 0;

  void reset()
  {
    x = 0;
    y = 0;
    explosionstatus = 0;
  }
};

class ExplosionManager
{
  private:
    Explosion explosions[MAX_ENEMY_ON_STAGE];
  public:
    void addexplosion(byte x, byte y, byte maxstep)
    {
      for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
        if (!((explosions[i].explosionstatus & 0b00010000) == 0b00010000))
        {
          explosions[i].x = x;
          explosions[i].y = y;
          explosions[i].explosionstatus = (byte)(maxstep << 5) | 0b00010001;
          break;
        }
    }

    void clr()
    {
      for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
        explosions[i].reset();
    }

    void draw()
    {
      for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
        if ((explosions[i].explosionstatus & 0b00010000) == 0b00010000)
        {
          if (arduboy.everyXFrames(15))
            explosions[i].explosionstatus += 1;

          if (((byte)(explosions[i].explosionstatus << 4) >> 4) > (explosions[i].explosionstatus >> 5))
            explosions[i].explosionstatus ^= 0b00010000;
          else
          {
            if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 1)
              arduboy.drawBitmap(explosions[i].x, explosions[i].y, explosion1, 8, 8, 1);
            else if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 2)
              arduboy.drawBitmap(explosions[i].x, explosions[i].y, explosion2, 8, 8, 1);
            else if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 3)
              arduboy.drawBitmap(explosions[i].x, explosions[i].y, explosion3, 8, 8, 1);
            else if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 4)
              arduboy.drawBitmap(explosions[i].x, explosions[i].y, explosion4, 8, 8, 1);
            else if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 5)
              arduboy.drawBitmap(explosions[i].x, explosions[i].y, explosion5, 8, 8, 1);
            else if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 6)
              arduboy.drawSlowXYBitmap(explosions[i].x, explosions[i].y, explosion6, 10, 10, 1);
            else if (((byte)(explosions[i].explosionstatus << 4)) >> 4 == 7)
              arduboy.drawSlowXYBitmap(explosions[i].x, explosions[i].y, explosion7, 12, 12, 1);
          }
        }
    }
};



