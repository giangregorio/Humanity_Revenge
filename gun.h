#define GUN_SIZE                   2
#define GUN_MIN_DIFF               12
#define GUN_MAXIMUM_PROJECTILES    120 / (GUN_SIZE + GUN_MIN_DIFF)
#define GUN_DAMAGE                 1

struct GunShoot
{
  public:
    byte x = 0;
    byte y = 0;
    bool visible = false;
};

struct GunShootItem
{
  public:
    GunShoot gunshoottop;
    GunShoot gunshootbottom;
};

class Gun
{
  private:
    uint16_t canshoot = 0;
    GunShootItem gun_shoots[GUN_MAXIMUM_PROJECTILES];
  public:
    void shoot(byte x, byte y)
    {
      if (canshoot == 0)
      {
        canshoot = arduboy.frameCount;

        GunShootItem gunshootitem;
        gunshootitem.gunshoottop.x = x;
        gunshootitem.gunshoottop.y = y;
        gunshootitem.gunshoottop.visible = true;

        gunshootitem.gunshootbottom.x = x;
        gunshootitem.gunshootbottom.y = y + 7;
        gunshootitem.gunshootbottom.visible = true;

        for (int i = GUN_MAXIMUM_PROJECTILES - 1; i > 0; i--)
          gun_shoots[i] = gun_shoots[i - 1];

        gun_shoots[0] = gunshootitem;
      }
    }

    void update()
    {
      if (canshoot != 0)
        if (arduboy.frameCount - canshoot >= GUN_MIN_DIFF)
          canshoot = 0;

      for (int i = 0; i < GUN_MAXIMUM_PROJECTILES; i++)
      {
        if (gun_shoots[i].gunshoottop.visible)
        {
          gun_shoots[i].gunshoottop.x += 2;

          if (gun_shoots[i].gunshoottop.x > 127)
            gun_shoots[i].gunshoottop.visible = false;
        }
        if (gun_shoots[i].gunshootbottom.visible)
        {
          gun_shoots[i].gunshootbottom.x += 2;

          if (gun_shoots[i].gunshootbottom.x > 127)
            gun_shoots[i].gunshootbottom.visible = false;
        }
      }
    }

    void draw()
    {
      for (int i = 0; i < GUN_MAXIMUM_PROJECTILES; i++)
      {
        if (gun_shoots[i].gunshoottop.visible)
          arduboy.drawFastHLine(gun_shoots[i].gunshoottop.x, gun_shoots[i].gunshoottop.y, GUN_SIZE, 1);

        if (gun_shoots[i].gunshootbottom.visible)
          arduboy.drawFastHLine(gun_shoots[i].gunshootbottom.x, gun_shoots[i].gunshootbottom.y, GUN_SIZE, 1);
      }
    }

    void clr()
    {
      for (int i = 0; i < GUN_MAXIMUM_PROJECTILES; i++)
      {
        GunShootItem gunshootitem;
        gun_shoots[i] = gunshootitem;
      }
    }

    void checkcollisions(EnemyManager &enemym, ExplosionManager &explosionm)
    {
      for (int i = 0; i < GUN_MAXIMUM_PROJECTILES; i++)
      {
        for (int j = 0; j < MAX_ENEMY_ON_STAGE; j++)
        {
          if ((enemym.enemies[j].enemystatus & 0b00001000) == 0b00001000)
          {
            if (gun_shoots[i].gunshoottop.visible)
              if (checkhitbox(gun_shoots[i].gunshoottop.x, gun_shoots[i].gunshoottop.y, GUN_SIZE, 1, enemym.enemies[j].x + enemym.enemies[j].hitboxXPadding, enemym.enemies[j].y + enemym.enemies[j].hitboxYPadding, enemym.enemies[j].hitboxWidth, enemym.enemies[j].hitboxHeight))
              {
                byte damage = 1;
                enemym.damageenemy(enemym.enemies[j], damage, explosionm);
                gun_shoots[i].gunshoottop.visible = false;
              }

            if (gun_shoots[i].gunshootbottom.visible)
              if (checkhitbox(gun_shoots[i].gunshootbottom.x, gun_shoots[i].gunshootbottom.y, GUN_SIZE, 1, enemym.enemies[j].x + enemym.enemies[j].hitboxXPadding, enemym.enemies[j].y + enemym.enemies[j].hitboxYPadding, enemym.enemies[j].hitboxWidth, enemym.enemies[j].hitboxHeight))
              {
                byte damage = 1;
                enemym.damageenemy(enemym.enemies[j], damage, explosionm);
                gun_shoots[i].gunshootbottom.visible = false;
              }
          }
        }
      }
    }
};


