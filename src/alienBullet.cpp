#include "alienBullet.h"
#include <iostream>

AlienBullet::AlienBullet(SDL_Texture *texture, const int &x, const int &y)
    : Entity(texture, x, y, ALIEN_BULLET_SPEED) {
  dx = -ALIEN_BULLET_SPEED;
  dy = 0;
}

AlienBullet::AlienBullet(SDL_Texture *texture, const int &x, const int &y,
                         const float &dx, const float &dy)
    : Entity(texture, x, y, ALIEN_BULLET_SPEED, dx, dy) {}

void AlienBullet::Update() {
  x += dx;
  y += dy;

  if ((x < 0) || (y < 0) || (y > kScreenHeight))
    health = 0;
}