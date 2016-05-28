#define BACKGROUND_STAR_NUM 6

struct Star
{
  byte x;
  byte y;
  // 1 visible, 2speed
  byte starstatus = 0b00000000;
};

class Background
{
  private:
    Star stars[BACKGROUND_STAR_NUM];
  public:
    void update()
    {
      for (int i = 0; i < BACKGROUND_STAR_NUM; i++)
        if ((stars[i].starstatus & 0b10000000) == 0b10000000)
        {
          if (stars[i].x <= 3)
            stars[i].starstatus ^= 0b10000000;
          else
            stars[i].x -= ((byte)(stars[i].starstatus << 1) >> 6);
        }

      if (arduboy.everyXFrames(15))
      {
        for (int i = 0; i < BACKGROUND_STAR_NUM; i++)
          if (!((stars[i].starstatus & 0b10000000) == 0b10000000))
          {
            Star star;
            star.x = 128;
            star.y = random(MIN_Y + 2, MAX_Y - 1);
            star.starstatus = 0b10000000 | (byte)((random(1, 3) << 5));
            stars[i] = star;
            break;
          }
      }
    }

    void draw()
    {
      for (int i = 0; i < BACKGROUND_STAR_NUM; i++)
        if ((stars[i].starstatus & 0b10000000) == 0b10000000)
          arduboy.drawPixel(stars[i].x, stars[i].y, 1);
    }

    void clr()
    {
      for (int i = 0; i < BACKGROUND_STAR_NUM; i++)
      {
        Star star;
        stars[i] = star;
      }
    }
};


