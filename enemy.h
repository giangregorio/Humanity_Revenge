#define EASY_ENEMY                 1
#define MEDIUM_ENEMY               2
#define BOSS_PART                  3

#define EASY_SKIN_1                1
#define EASY_SKIN_2                2
#define EASY_SKIN_3                3

#define MOVEMENT_TYPE_HORIZONTAL   1
#define MOVEMENT_TYPE_SINUSOIDE    2
#define MOVEMENT_TYPE_KAMIKAZE     3

#define MAX_ENEMY_SHOOTS           8

const float pi = 3.141593;

struct EnemyShoot
{
  int x, y;
  bool visible = false;
  // 1 positive, 7 angle
  byte shootstatus = 0;
};

struct Enemy
{
  // 2 type, 2movtype, 1visible, 2enemyskin, 1bombed
  byte enemystatus = 0;
  byte x, y;
  byte hitboxWidth, hitboxHeight, hitboxXPadding, hitboxYPadding;

  byte enemylife;
  byte firstY = 0;
  byte randomseed;
};

#include "bosses.h"

class EnemyManager
{
  private:
    byte easyenemyonstage = 0;
    byte mediumenemyonstage = 0;

    byte shootsonstage = 0;
  public:
    Enemy enemies[MAX_ENEMY_ON_STAGE];
    EnemyShoot shoots[MAX_ENEMY_SHOOTS];
    Boss boss;

    void damageenemy(Enemy &enemy, byte &damage, ExplosionManager &explosionm)
    {
      if ((enemy.enemystatus & 0b00001000) == 0b00001000)
      {
        if (enemy.enemylife <= damage)
        {
          damage = damage - enemy.enemylife;
          enemy.enemylife = 0;
        }
        else
        {
          enemy.enemylife -= damage;
          damage = 0;
        }

        if (enemy.enemylife == 0)
        {
          byte maxstep = 3;
          byte score = 1;
          if ((enemy.enemystatus >> 6) == MEDIUM_ENEMY)
          {
            maxstep = 5;
            score = 3;
          }
          else if ((enemy.enemystatus >> 6) == BOSS_PART)
          {
            maxstep = 7;
            score = 25;
          }

          explosionm.addexplosion(enemy.x, enemy.y, maxstep);
          enemy.enemystatus ^= 0b00001000;
          decreaseenemies(enemy);
          UpdateGlobalScore(score);
        }
      }
    }

    void spawn(int shipX, int shipY)
    {
      if (currentlevel <= 5)
      {
        if (arduboy.everyXFrames(10))
        {
          byte inc = 0;
          byte y = random(MIN_Y, 56);

          // easy
          byte allowedenemies = MAX_EASY_ENEMY_ON_STAGE  - (5 - currentlevel) - easyenemyonstage;
          if (random(0, 5 - min(currentlevel, 3)) == 0 && allowedenemies > 0)
          {
            //easy spawn
            byte tospawn = random(1, allowedenemies + 1);
            easyenemyonstage += tospawn;

            byte movementtype = MOVEMENT_TYPE_HORIZONTAL;
            byte skin = (byte)(EASY_SKIN_1 << 1);
            if (currentlevel > 1)
            {
              long rndskin = random(0, min(currentlevel, 3));
              if (rndskin == 1)
              {
                skin = (byte)(EASY_SKIN_2 << 1);
                movementtype = MOVEMENT_TYPE_SINUSOIDE;
              }
              else if (rndskin == 2 && (tospawn == 1 || tospawn == 2))
              {
                skin = (byte)(EASY_SKIN_3 << 1);
                movementtype = MOVEMENT_TYPE_KAMIKAZE;
              }
            }

            for (int i = 0; i < tospawn; i++)
            {
              Enemy easyenemy;
              easyenemy.enemystatus = ((byte)(EASY_ENEMY << 6) | (byte)(movementtype << 4) | skin | 0b00001000);
              easyenemy.x = 128 + inc;
              easyenemy.y = y;
              easyenemy.firstY = y;
              easyenemy.hitboxXPadding = 1;
              easyenemy.hitboxYPadding = 1;
              easyenemy.hitboxWidth = 5;
              easyenemy.hitboxHeight = 6;
              easyenemy.enemylife = 1;
              easyenemy.randomseed = random(0, 100);

              inc += 7;

              for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
                if (!((enemies[i].enemystatus & 0b00001000) == 0b00001000))
                {
                  enemies[i] = easyenemy;
                  break;
                }
            }
          }

          // medium
          allowedenemies = 0;
          if (currentlevel > 3)
            allowedenemies = random(0, currentlevel - 3 - mediumenemyonstage + 1);

          if (random(0, 5) == 0 && allowedenemies > 0)
          {
            byte tospawn = random(1, allowedenemies + 1);
            mediumenemyonstage += tospawn;
            byte movementtype = MOVEMENT_TYPE_HORIZONTAL;
            if (currentlevel > 4)
              movementtype = random(MOVEMENT_TYPE_HORIZONTAL, MOVEMENT_TYPE_SINUSOIDE + 1);

            for (int i = 0; i < tospawn; i++)
            {
              Enemy mediumenemy;
              mediumenemy.enemystatus = (byte)(MEDIUM_ENEMY << 6) | (byte)(movementtype << 4) | 0b00001000;
              mediumenemy.x = 128 + inc;
              mediumenemy.y = y;
              mediumenemy.firstY = y;
              mediumenemy.hitboxXPadding = 0;
              mediumenemy.hitboxYPadding = 0;
              mediumenemy.hitboxWidth = 8;
              mediumenemy.hitboxHeight = 8;
              mediumenemy.enemylife = 3;
              mediumenemy.randomseed = random(0, 100);

              inc += 9;

              for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
                if (!((enemies[i].enemystatus & 0b00001000) == 0b00001000))
                {
                  enemies[i] = mediumenemy;
                  break;
                }
            }
          }
        }
      } else
      {
        if (!((boss.bossstatus & 0b00001000) == 0b00001000))
          boss.spawnboss(enemies);
      }

      //shoot
      if (arduboy.everyXFrames(10))
      {
        if (shootsonstage < MAX_ENEMY_SHOOTS)
          for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
          {
            if (((enemies[i].enemystatus & 0b00001000) == 0b00001000) && (enemies[i].x > (shipX / 100) + 8 + 24) && enemies[i].x < 128 &&
                // must not be kamikaze movement
                !(((byte)(enemies[i].enemystatus << 2) >> 6 ) == MOVEMENT_TYPE_KAMIKAZE))
            {
              if (random(0, 16 - currentlevel) == 0)
              {
                EnemyShoot enemyshoot;
                enemyshoot.x = enemies[i].x * 100;
                enemyshoot.y = enemies[i].y * 100 + 3;
                enemyshoot.visible = true;

                if (enemyshoot.y > (shipY + 300))
                  enemyshoot.shootstatus = 0b10000000 | (byte)(atan2(enemyshoot.y - (shipY + 300), enemyshoot.x - (shipX + 300)) * 180 / pi);
                else if (enemyshoot.y == (shipY + 300))
                  enemyshoot.shootstatus = 0b00000000;
                else
                  enemyshoot.shootstatus = 0b00000000 | (byte)abs(atan2((shipY + 300) - enemyshoot.y, enemyshoot.x - (shipX + 300)) * 180 / pi);

                for (int j = 0; j < MAX_ENEMY_SHOOTS; j++)
                  if (!shoots[j].visible)
                  {
                    shoots[j] = enemyshoot;
                    shootsonstage++;
                    break;
                  }

                break;
              }
            }
          }
      }
    }

    void update(int shipX, int shipY, ExplosionManager &explosionm)
    {
      if (currentlevel <= 5)
      {
        for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
        {
          if ((enemies[i].enemystatus & 0b00001000) == 0b00001000)
          {
            if (enemies[i].x <= 0)
            {
              enemies[i].enemystatus ^= 0b00001000;
              decreaseenemies(enemies[i]);
            }
            else
            {
              if (((byte)(enemies[i].enemystatus << 2) >> 6 ) == MOVEMENT_TYPE_HORIZONTAL)
                enemies[i].x--;
              else if (((byte)(enemies[i].enemystatus << 2) >> 6 ) == MOVEMENT_TYPE_SINUSOIDE)
              {
                enemies[i].x--;
                enemies[i].y = (6 * sin((arduboy.frameCount + enemies[i].randomseed) * 0.5 * pi / 33)) + enemies[i].firstY;
                if (enemies[i].y < MIN_Y)
                  enemies[i].y = MIN_Y;
                else if (enemies[i].y > 56)
                  enemies[i].y = 56;
              } else if (((byte)(enemies[i].enemystatus << 2) >> 6 ) == MOVEMENT_TYPE_KAMIKAZE)
              {
                enemies[i].x -= 2;
                if ((shipX / 100) < enemies[i].x)
                {
                  if ((shipY / 100) > enemies[i].y)
                    enemies[i].y++;
                  else
                    enemies[i].y--;
                }
              }
            }
          }
        }
      }
      else
      {
        boss.update(enemies);

        if (!((enemies[0].enemystatus & 0b00001000) == 0b00001000))
        {
          if ((enemies[1].enemystatus & 0b00001000) == 0b00001000)
            damageenemy(enemies[1], enemies[1].enemylife, explosionm);
          if ((enemies[2].enemystatus & 0b00001000) == 0b00001000)
            damageenemy(enemies[2], enemies[2].enemylife, explosionm);
          if ((enemies[3].enemystatus & 0b00001000) == 0b00001000)
            damageenemy(enemies[3], enemies[3].enemylife, explosionm);
          if ((enemies[4].enemystatus & 0b00001000) == 0b00001000)
            damageenemy(enemies[4], enemies[4].enemylife, explosionm);
        }
      }

      for (int i = 0; i < MAX_ENEMY_SHOOTS; i++)
      {
        if (shoots[i].visible)
        {
          shoots[i].x -= 1.5 * cos(((byte)(shoots[i].shootstatus << 1) >> 1) * pi / 180) * 100;
          if (shoots[i].x <= 0)
            shoots[i].visible = false;

          if (shoots[i].visible)
          {
            if ((shoots[i].shootstatus & 0b10000000) == 0b10000000)
            {
              shoots[i].y -= 1.5 * sin(((byte)(shoots[i].shootstatus << 1) >> 1) * pi / 180) * 100;
              if (shoots[i].y < MIN_Y * 100)
                shoots[i].visible = false;
            }
            else
            {
              shoots[i].y += 1.5 * sin(((byte)(shoots[i].shootstatus << 1) >> 1) * pi / 180) * 100;
              if (shoots[i].y > MAX_Y * 100)
                shoots[i].visible = false;
            }
          }

          if (!shoots[i].visible)
            shootsonstage--;
        }
      }
    }

    bool bossdead()
    {
      return boss.isdead(enemies);
    }

    void draw()
    {
      if (currentlevel <= 5)
      {
        for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
        {
          if ((enemies[i].enemystatus & 0b00001000) == 0b00001000 && enemies[i].x < 128)
          {
            if ((enemies[i].enemystatus >> 6) == EASY_ENEMY)
            {
#ifdef DRAWONLYHITBOX
              arduboy.drawRect(enemies[i].x + enemies[i].hitboxXPadding, enemies[i].y + enemies[i].hitboxYPadding, enemies[i].hitboxWidth, enemies[i].hitboxHeight, 1);
#else
              if (((byte)(enemies[i].enemystatus << 5) >> 6 ) == EASY_SKIN_1)
                arduboy.drawBitmap(enemies[i].x, enemies[i].y, easy_enemy1, 8, 8, 1);
              else if (((byte)(enemies[i].enemystatus << 5) >> 6 ) == EASY_SKIN_2)
                arduboy.drawBitmap(enemies[i].x, enemies[i].y, easy_enemy2, 8, 8, 1);
              else if (((byte)(enemies[i].enemystatus << 5) >> 6 ) == EASY_SKIN_3)
                arduboy.drawBitmap(enemies[i].x, enemies[i].y, easy_enemy3, 8, 8, 1);
#endif
            }
            else if ((enemies[i].enemystatus >> 6) == MEDIUM_ENEMY)
            {
#ifdef DRAWONLYHITBOX
              arduboy.drawRect(enemies[i].x + enemies[i].hitboxXPadding, enemies[i].y + enemies[i].hitboxYPadding, enemies[i].hitboxWidth, enemies[i].hitboxHeight, 1);
#else
              arduboy.drawBitmap(enemies[i].x, enemies[i].y, medium_enemy1, 8, 8, 1);
#endif
            }
          }
        }
      } else
        boss.draw(enemies);

      for (int i = 0; i < MAX_ENEMY_SHOOTS; i++)
        if (shoots[i].visible)
        {
#ifdef DRAWONLYHITBOX
          arduboy.drawLine(shoots[i].x / 100, shoots[i].y / 100, shoots[i].x / 100 + 1, shoots[i].y / 100, 1);
          arduboy.drawLine(shoots[i].x / 100, shoots[i].y / 100 + 1, shoots[i].x / 100 + 1, shoots[i].y / 100 + 1, 1);
#else
          arduboy.drawLine((shoots[i].x / 100) - 1, shoots[i].y / 100, (shoots[i].x / 100) + 1, shoots[i].y / 100, 1);
          arduboy.drawLine(shoots[i].x / 100, (shoots[i].y / 100) + 1, shoots[i].x / 100, (shoots[i].y / 100) - 1, 1);
#endif
        }
    }

    void clrshoot(bool force = false)
    {
      if (shootsonstage > 0 || force)
        for (int i = 0; i < MAX_ENEMY_SHOOTS; i++)
        {
          EnemyShoot enemyshoot;
          shoots[i] = enemyshoot;
        }
      shootsonstage = 0;
    }

    void clr(bool force = false)
    {
      easyenemyonstage = 0;
      mediumenemyonstage = 0;

      for (int i = 0; i < MAX_ENEMY_ON_STAGE; i++)
      {
        Enemy enemy;
        enemies[i] = enemy;
      }

      clrshoot(force);

      Boss _boss;
      boss = _boss;
    }

    void decreaseenemies(Enemy &enemy)
    {
      if ((enemy.enemystatus >> 6) == EASY_ENEMY)
        easyenemyonstage--;
      else if ((enemy.enemystatus >> 6) == MEDIUM_ENEMY)
        mediumenemyonstage--;
    }

    bool arevisibleenemies()
    {
      return easyenemyonstage > 0 || mediumenemyonstage > 0;
    }
};
