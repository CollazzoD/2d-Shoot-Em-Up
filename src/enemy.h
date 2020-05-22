#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "renderer.h"
#include "player.h"
#include "alienBullet.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <list>
#include <memory>
#include <random>

constexpr int ENEMY_RELOAD{64};
constexpr int ENEMY_MAX_SPEED{6};
constexpr int ENEMY_MIN_SPEED{2};

class Enemy : public Entity {
public:
  Enemy(SDL_Texture *texture, const int &x, const int &y, const int &speed,
        AlienBullet bullet_forge,
        std::list<std::unique_ptr<AlienBullet>> &enemy_bullets, const Player* player);
  ~Enemy() { std::cout << "Enemy destroyed " << std::endl;};
  Enemy(const Enemy &source) = delete;
  Enemy &operator=(const Enemy &source) = delete;
  Enemy(Enemy &&source) = default;
  Enemy &operator=(Enemy &&source) = default;

  void Update() override;
  void TargetPosition(Entity* e);

private:
  void CheckPosition();
  void Fire();
  void CalcSlope(float &dx, float &dy);

  int bullet_dx;
  int bullet_dy;
  const Player* player;

  AlienBullet bullet_forge; // used to create other bullets
  std::list<std::unique_ptr<AlienBullet>> &enemy_bullets;
  int reload;
};

#endif