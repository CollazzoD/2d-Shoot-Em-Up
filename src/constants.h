#ifndef CONSTANT_H
#define CONSTANT_H

#include <iostream>

constexpr int kScreenWidth{1280};
constexpr int kScreenHeight{720};
constexpr std::size_t kFramesPerSecond{60};
constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};

constexpr int ENEMY_SPAWN_TIMER = 120;
constexpr int RESET_STAGE_TIMER = 300;

constexpr int MAX_STARS{500};
constexpr int MAX_STAR_SPEED{8};

constexpr int PLAYER_SPEED{4};
constexpr int PLAYER_INITIAL_X{100};
constexpr int PLAYER_INITIAL_Y{330};

constexpr int ENEMY_RELOAD{64};
constexpr int ENEMY_MAX_SPEED{6};
constexpr int ENEMY_MIN_SPEED{2};

constexpr int ALIEN_BULLET_SPEED{8};

constexpr int BULLET_SPEED{16};

constexpr int GLYPH_HEIGHT{28};
constexpr int GLYPH_WIDTH{18};

constexpr int ENEMY_DESTROYED_SCORE{100};

#endif