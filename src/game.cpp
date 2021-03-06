#include "game.h"
#include <SDL2/SDL.h>
#include <functional>
#include <iostream>
#include <random>

Game::Game(Renderer &renderer) {
  player_texture =
      std::make_unique<Texture>(renderer.LoadTexture("../gfx/player.png"));
  bullet_texture = std::make_unique<Texture>(
      renderer.LoadTexture("../gfx/playerBullet.png"));
  enemy_texture =
      std::make_unique<Texture>(renderer.LoadTexture("../gfx/enemy.png"));
  enemy_bullet_texture =
      std::make_unique<Texture>(renderer.LoadTexture("../gfx/alienBullet.png"));
  explosion_texture =
      std::make_unique<Texture>(renderer.LoadTexture("../gfx/explosion.png"));

  player = std::make_unique<Player>(player_texture.get(), bullet_texture.get(),
                                    bullets);

  engine = std::mt19937(dev());
  enemy_random_speed =
      std::uniform_int_distribution<int>(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED);

  int enemy_max_pos = kScreenHeight - enemy_texture->GetTextureHeight();
  enemy_random_pos = std::uniform_int_distribution<int>(0, enemy_max_pos);

  zero_to_4 = std::uniform_int_distribution<int>(0, 4);
  zero_to_5 = std::uniform_int_distribution<int>(0, 5);
  zero_to_10 = std::uniform_int_distribution<int>(0, 10);
  zero_to_12 = std::uniform_int_distribution<int>(0, 12);
  zero_to_32 = std::uniform_int_distribution<int>(0, 32);
  zero_to_fps = std::uniform_int_distribution<int>(0, kFramesPerSecond);

  ResetStage();
}

void Game::ResetStage() {
  bullets.erase(begin(bullets), end(bullets));
  enemies.erase(begin(enemies), end(enemies));
  enemies_bullets.erase(begin(enemies_bullets), end(enemies_bullets));
  explosions.erase(begin(explosions), end(explosions));
  space_debris.erase(begin(space_debris), end(space_debris));
  player->Init();
  enemy_spawn_timer = ENEMY_SPAWN_TIMER;
  reset_stage_timer = RESET_STAGE_TIMER;
  highscore_table.PushScore(score);
  score = 0;
  highscore = highscore_table.GetHighScore();
}

void Game::RenderGameEntities(Renderer &renderer) {
  if (player->GetHealth() == 1)
    renderer.Render(player.get());

  for (auto const &bullet : bullets)
    if (bullet->GetHealth() == 1)
      renderer.Render(bullet.get());

  for (auto const &enemy : enemies)
    if (enemy->GetHealth() == 1)
      renderer.Render(enemy.get());

  for (auto const &bullet : enemies_bullets)
    if (bullet->GetHealth() == 1)
      renderer.Render(bullet.get());

  for (auto const &debris : space_debris)
    if (debris->GetHealth() > 0)
      renderer.Render(dynamic_cast<Debris *>(debris.get()));

  for (auto const &explosion : explosions)
    renderer.Render(dynamic_cast<Explosion *>(explosion.get()));

  renderer.Render(game_stars);
}

void Game::Playing(Controller const &controller, Renderer &renderer, bool &running) {
  controller.HandleInput(running, playing, player.get());

  Update();
  RenderGameEntities(renderer);
  renderer.DrawHud(score, highscore);

  if (player->GetHealth() <= 0) {
    reset_stage_timer--;
    if (reset_stage_timer <= 0)
      ResetStage();
  }
}

void Game::ShowHighscore(Controller const &controller, Renderer &renderer, bool &running) {
  controller.HandleInput(running, playing);
  renderer.PrintHighscore(highscore_table);
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    renderer.PrepareScene();

    if (playing)
      Playing(controller, renderer, running);
    else
      ShowHighscore(controller, renderer, running);

    renderer.PresentScene();

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::SpawnEnemy() {
  enemy_spawn_timer--;
  if (enemy_spawn_timer <= 0) {
    auto enemy = std::make_unique<Enemy>(
        enemy_texture.get(), kScreenWidth, enemy_random_pos(engine),
        enemy_random_speed(engine), enemy_bullet_texture.get(), enemies_bullets,
        player.get());
    enemies.push_front(std::move(enemy));
    enemy_spawn_timer = ENEMY_SPAWN_TIMER;
  }
}

void Game::UpdateEntities(std::list<std::unique_ptr<Entity>> &list) {
  for (auto element = begin(list); element != end(list);) {
    if (!*element) {
      element = list.erase(element);
    } else if ((*element)->GetHealth() <= 0) {
      element = list.erase(element);
    } else {
      (*element)->Update();
      element++;
    }
  }
}

void Game::Update() {
  // Update player
  player->Update();

  SpawnEnemy();
  UpdateEntities(bullets);
  UpdateEntities(enemies);
  UpdateEntities(enemies_bullets);
  UpdateEntities(explosions);
  UpdateEntities(space_debris);
  game_stars.Update();

  CheckCollision();
}

bool Game::Collision(const Entity *e1, const Entity *e2) {
  int max_x = std::max<int>(e1->GetX(), e2->GetX());
  int min_xw =
      std::min<int>(e1->GetX() + e1->GetWidth(), e2->GetX() + e2->GetWidth());

  int max_y = std::max<int>(e1->GetY(), e2->GetY());
  int min_yh =
      std::min<int>(e1->GetY() + e1->GetHeight(), e2->GetY() + e2->GetHeight());

  return (max_x < min_xw) && (max_y < min_yh);
}

void Game::CheckCollision() {
  // Player bullet's enemies collision
  for (const auto &enemy : enemies)
    for (const auto &bullet : bullets)
      if (Collision(enemy.get(), bullet.get())) {
        enemy->Hit();
        bullet->Hit();
        score += ENEMY_DESTROYED_SCORE;
        AddExplosions(enemy->GetX(), enemy->GetY(), 32);
        AddDebris(enemy.get());
      }

  // Enemies bullet's Player collision
  for (const auto &bullet : enemies_bullets)
    if (Collision(player.get(), bullet.get())) {
      player->Hit();
      bullet->Hit();
    }
}

void Game::AddExplosions(const int &x, const int &y, const int &num) {
  for (int i = 0; i < num; i++) {
    float exp_x = x + zero_to_32(engine) - zero_to_32(engine);
    float exp_y = y + zero_to_32(engine) - zero_to_32(engine);
    float dx = zero_to_10(engine) - zero_to_10(engine);
    float dy = zero_to_10(engine) - zero_to_10(engine);
    int r, g, b, a;
    r = g = b = a = 0;

    dx /= 10;
    dy /= 10;

    switch (zero_to_4(engine)) {
    case 0:
      r = 255;
      break;

    case 1:
      r = 255;
      g = 128;
      break;

    case 2:
      r = 255;
      g = 255;
      break;

    default:
      r = 255;
      g = 255;
      b = 255;
      break;
    }

    a = zero_to_fps(engine) * 3;

    auto explosion = std::make_unique<Explosion>(explosion_texture.get(), exp_x,
                                                 exp_y, dx, dy, r, g, b, a);
    explosions.push_front(std::move(explosion));
  }
}

void Game::AddDebris(const Entity *e) {
  int x, y, w, h;
  float debris_x, debris_y, debris_dx, debris_dy;
  int debris_life;
  SDL_Rect rect;

  w = e->GetWidth() / 2;
  h = e->GetHeight() / 2;
  for (y = 0; y <= h; y += h) {
    for (x = 0; x <= w; x += w) {
      debris_x = e->GetX() + e->GetWidth() / 2;
      debris_y = e->GetY() + e->GetHeight() / 2;
      debris_dx = zero_to_5(engine) - zero_to_5(engine);
      debris_dy = -(5 + zero_to_12(engine));
      debris_life = kFramesPerSecond * 2;

      rect.x = x;
      rect.y = y;
      rect.w = w;
      rect.h = h;

      auto debris =
          std::make_unique<Debris>(e->GetTexture(), debris_x, debris_y,
                                   debris_dx, debris_dy, debris_life, rect);
      space_debris.push_front(std::move(debris));
    }
  }
}