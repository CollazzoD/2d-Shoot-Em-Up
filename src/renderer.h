#ifndef RENDERER_H
#define RENDERER_H

#include "constants.h"
#include "entity.h"
#include "star.h"
#include "texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>
#include <string>

class Renderer {
public:
  Renderer();
  ~Renderer();

  Renderer(const Renderer &source) = delete;
  Renderer &operator=(const Renderer &source) = delete;
  Renderer(Renderer &&source);
  Renderer &operator=(Renderer &&source);

  void PrepareScene();
  void PresentScene();
  void UpdateWindowTitle(const int &x, const int &y, const int &fps);
  void RenderTexture(const Entity *entity);
  void RenderBackground();
  void RenderStars(const Stars &stars);
  void DrawHud(const int &score);
  SDL_Texture *LoadTexture(const std::string &filename) const;

private:
  void DrawText(const int &x, const int &y, const int &r, const int &g,
                const int &b, const std::string &text);
  void UpdateBackground();
  void BlitRect(SDL_Texture *texture, const SDL_Rect *src, const int &x,
                const int &y);

  std::unique_ptr<Texture> background_texture;
  std::unique_ptr<Texture> font_texture;

  int backgroundX{0};

  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  int screen_width;
  int screen_height;
};

#endif
