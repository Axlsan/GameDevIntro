#include <cassert>
#include <string>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_image.h"
#include "image.h"
#include "arena.h"
using namespace std;

const char* DIRECTORY = "assets/sprites/";
const char* FALLBACK = "assets/sprites/dog.png";


Image* AssetManagement::LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* name){
  string path = DIRECTORY;
  path = path.append(name);
  SDL_Surface* surface = IMG_Load(path.c_str());
  if(surface == nullptr){
    SDL_Log("Could not load %s: %s", path.c_str(), SDL_GetError());
    surface = IMG_Load(FALLBACK);
   }
  if(surface == nullptr){
    SDL_Log("Could not load fallback texture %s: %s", FALLBACK, SDL_GetError());
    return nullptr;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if(texture == nullptr){
    SDL_Log("Could not create texture from %s: %s", path.c_str(), SDL_GetError());
    SDL_DestroySurface(surface);
    return nullptr;
  }
  Image* img = (Image*)Memory::Allocate(arena, sizeof(Image));
  if (img == nullptr) {
      SDL_Log("Memory::Allocate failed allocating Image");
      SDL_DestroyTexture(texture);
      SDL_DestroySurface(surface);
      return nullptr;
  }
  img->texture = texture;
  img->height = texture->h;
  img->width = texture->w;
  SDL_DestroySurface(surface);
  return img;
}
