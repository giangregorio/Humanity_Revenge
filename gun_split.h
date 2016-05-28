struct GunSplitShootItem
{
  public:
    GunShoot gunshoottop;
    GunShoot gunshootmiddle;
    GunShoot gunshootbottom;
};

class GunSplit
{
  private:
    bool top = true;
    uint16_t canshoot = 0;
    GunSplitShootItem gun_shoots[GUN_MAXIMUM_PROJECTILES];
  public:
    void shoot(byte x, byte y)
    {
      if (canshoot == 0)
      {
        canshoot = arduboy.frameCount;

        GunSplitShootItem gunsplitshootitem;
        gunsplitshootitem.gunshoottop.x = x;
        gunsplitshootitem.gunshoottop.y = y;
        gunsplitshootitem.gunshoottop.visible = true;

        gunsplitshootitem.gunshootmiddle.x = x;
        if (top)
          gunsplitshootitem.gunshootmiddle.y = y + 4;
        else
          gunsplitshootitem.gunshootmiddle.y = y + 3;
        gunsplitshootitem.gunshootmiddle.visible = true;

        top = !top;

        gunsplitshootitem.gunshootbottom.x = x;
        gunsplitshootitem.gunshootbottom.y = y + 7;
        gunsplitshootitem.gunshootbottom.visible = true;

        for (int i = GUN_MAXIMUM_PROJECTILES - 1; i > 0; i--)
          gun_shoots[i] = gun_shoots[i - 1];

        gun_shoots[0] = gunsplitshootitem;
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
          gun_shoots[i].gunshoottop.y -= 1;

          if (gun_shoots[i].gunshoottop.x > 128)
            gun_shoots[i].gunshoottop.visible = false;
        }

        if (gun_shoots[i].gunshootmiddle.visible)
        {
          gun_shoots[i].gunshootmiddle.x += 2;

          if (gun_shoots[i].gunshootmiddle.x > 128)
            gun_shoots[i].gunshootmiddle.visible = false;
        }

        if (gun_shoots[i].gunshootbottom.visible)
        {
          gun_shoots[i].gunshootbottom.x += 2;
          gun_shoots[i].gunshootbottom.y += 1;

          if (gun_shoots[i].gunshootbottom.x > 128)
            gun_shoots[i].gunshootbottom.visible = false;
        }

      }
    }

    void draw()
    {
      for (int i = 0; i < GUN_MAXIMUM_PROJECTILES; i++)
      {
        if (gun_shoots[i].gunshoottop.visible)
          arduboy.drawLine(gun_shoots[i].gunshoottop.x, gun_shoots[i].gunshoottop.y, gun_shoots[i].gunshoottop.x + GUN_SIZE - 1, gun_shoots[i].gunshoottop.y - 1, 1);

        if (gun_shoots[i].gunshootmiddle.visible)
          arduboy.drawFastHLine(gun_shoots[i].gunshootmiddle.x, gun_shoots[i].gunshootmiddle.y, GUN_SIZE, 1);

        if (gun_shoots[i].gunshootbottom.visible)
          arduboy.drawLine(gun_shoots[i].gunshootbottom.x, gun_shoots[i].gunshootbottom.y, gun_shoots[i].gunshootbottom.x + GUN_SIZE - 1, gun_shoots[i].gunshootbottom.y + 1, 1);
      }
    }

    void clr()
    {
      for (int i = 0; i < GUN_MAXIMUM_PROJECTILES; i++)
      {
        GunSplitShootItem gunshootitem;
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

            if (gun_shoots[i].gunshootmiddle.visible)
              if (checkhitbox(gun_shoots[i].gunshootmiddle.x, gun_shoots[i].gunshootmiddle.y, GUN_SIZE, 1, enemym.enemies[j].x + enemym.enemies[j].hitboxXPadding, enemym.enemies[j].y + enemym.enemies[j].hitboxYPadding, enemym.enemies[j].hitboxWidth, enemym.enemies[j].hitboxHeight))
              {
                byte damage = 1;
                enemym.damageenemy(enemym.enemies[j], damage, explosionm);
                gun_shoots[i].gunshootmiddle.visible = false;
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


