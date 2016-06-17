class Bomb
{
  private:
  public:
    bool visible = false;
    byte x = 0;
    byte y = 0;
    byte radius = 0;

    void update(EnemyManager &enemym)
    {
      if (visible)
      {
        radius += 2;

        if (radius > 200)
        {
          visible = false;

          for (int j = 0; j < MAX_ENEMY_ON_STAGE; j++)
            if ((enemym.enemies[j].enemystatus & 0b00001001) == 0b00001001)
              enemym.enemies[j].enemystatus ^= 0b00000001;
        } else
          enemym.clrshoot();
      }
    }

    void draw()
    {
      if (visible)
      {
        arduboy.drawCircle(x, y, radius, 1);
        arduboy.drawCircle(x, y, radius - 4, 1);
      }
    }

    void clr()
    {
      visible = false;
    }

    void checkcollisions(EnemyManager &enemym, ExplosionManager &explosionm)
    {
      if (visible)
        for (int j = 0; j < MAX_ENEMY_ON_STAGE; j++)
          if ((enemym.enemies[j].enemystatus & 0b00001001) == 0b00001000)
            if (checkhitboxcr(x, y, radius, enemym.enemies[j].x + enemym.enemies[j].hitboxXPadding, enemym.enemies[j].y + enemym.enemies[j].hitboxYPadding, enemym.enemies[j].hitboxWidth, enemym.enemies[j].hitboxHeight))
            {
              byte damage = 3;
              enemym.damageenemy(enemym.enemies[j], damage, explosionm);
              enemym.enemies[j].enemystatus |= 0b00000001;
            }
    }
};

