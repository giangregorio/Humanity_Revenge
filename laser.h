#define LASER_DAMAGE                4
#define LASER_SIZE                  10
#define LASER_DELAY                 30
#define LASER_MAXIMUM_PROJECTILES   120 / (LASER_SIZE + LASER_DELAY)

struct LaserShoot
{
  public:
    byte x = 0;
    byte y = 0;
    bool visible = false;
    byte damage = LASER_DAMAGE;
    byte width = 0;

    void reset()
    {
      x = 0;
      y = 0;
      visible = false;
      damage = LASER_DAMAGE;
      width = 0;
    }
};

class Laser
{
  private:
    uint16_t canshoot = 0;
    LaserShoot laser_shoots[LASER_MAXIMUM_PROJECTILES];
  public:
    void shoot(byte x, byte y)
    {
      if (canshoot == 0)
      {
        canshoot = arduboy.frameCount;

        LaserShoot lasershoot;
        lasershoot.x = x;
        lasershoot.y = y + 3;
        lasershoot.visible = true;

        for (int i = LASER_MAXIMUM_PROJECTILES - 1; i > 0; i--)
          laser_shoots[i] = laser_shoots[i - 1];

        laser_shoots[0] = lasershoot;
      }
    }

    void update()
    {
      if (canshoot != 0)
        if (arduboy.frameCount - canshoot >= LASER_DELAY)
          canshoot = 0;

      for (int i = 0; i < LASER_MAXIMUM_PROJECTILES; i++)
      {
        if (laser_shoots[i].visible)
        {
          if (laser_shoots[i].width < LASER_SIZE)
            laser_shoots[i].width += 2;
          else
            laser_shoots[i].x += 2;

          if (laser_shoots[i].x > 128)
            laser_shoots[i].visible = false;
        }
      }
    }

    void draw()
    {
      for (int i = 0; i < LASER_MAXIMUM_PROJECTILES; i++)
      {
        if (laser_shoots[i].visible)
        {
          arduboy.drawFastHLine(laser_shoots[i].x, laser_shoots[i].y, laser_shoots[i].width, 1);
          arduboy.drawFastHLine(laser_shoots[i].x, laser_shoots[i].y + 1, laser_shoots[i].width, 1);
        }
      }
    }

    void clr()
    {
      for (int i = 0; i < LASER_MAXIMUM_PROJECTILES; i++)
        laser_shoots[i].reset();
    }

    void checkcollisions(EnemyManager &enemym, ExplosionManager &explosionm)
    {
      for (int i = 0; i < LASER_MAXIMUM_PROJECTILES; i++)
      {
        for (int j = 0; j < MAX_ENEMY_ON_STAGE; j++)
        {
          if ((enemym.enemies[j].enemystatus & 0b00001000) == 0b00001000)
          {
            if (laser_shoots[i].visible)
            {
              if (checkhitbox(laser_shoots[i].x, laser_shoots[i].y, laser_shoots[i].width, 2, enemym.enemies[j].x + enemym.enemies[j].hitboxXPadding, enemym.enemies[j].y + enemym.enemies[j].hitboxYPadding, enemym.enemies[j].hitboxWidth, enemym.enemies[j].hitboxHeight))
              {
                enemym.damageenemy(enemym.enemies[j], laser_shoots[i].damage, explosionm);
                if (laser_shoots[i].damage == 0)
                  laser_shoots[i].visible = false;
              }
            }
          }
        }
      }
    }
};



