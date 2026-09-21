#pragma once

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"

struct Input{
  const bool* keysCurrent;
  const bool* keysPrevious;
  float* keysHeldTime;

  SDL_MouseButtonFlags mouseCurrent;
  SDL_MouseButtonFlags mousePrevious;
  float* mouseHeldTime;
  float mouseX;
  float mouseY;
  
};

enum class MouseButtons{
  LEFT = 0,
  MIDDLE = 1,
  RIGHT = 2 
};

bool KeyPressed(const Input* input, SDL_Scancode key);
bool KeyHeld(const Input* input, SDL_Scancode key);
bool KeyReleased(const Input* input, SDL_Scancode key);
bool KeyHeldForTime(const Input* input, SDL_Scancode key, float minLength);

void UpdateKeys(Input* input, float dt);
void ResetKeyHeldTime(Input* input, SDL_Scancode key);
void ResetAll(Input* input);


bool MousePressed(const Input* input, MouseButtons button);
bool MouseReleased(const Input* input, MouseButtons button);
bool MouseHeld(const Input* input, MouseButtons button);
bool MouseHeldForTime(const Input* input, MouseButtons button, float minLength);

void UpdateMouse(Input* input, float dt);

