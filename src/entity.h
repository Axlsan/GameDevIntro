#pragma once

#include <cassert>
#include <cstdint>

enum Behaviour : uint32_t{
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2
};

enum class ID : uint8_t {
  NONE = 0,
  GROUND = 4,
  WALL = 5,
  DEMON = 6,
  ROCK = 7,
  MEDUSA = 1,
  SIREN = 2,
  GOLEM = 3
  
  /*NONE = 0,
  GROUND = 4,
  WALL = 5,
  PLAYER = 6,
  BOX = 7*/
};

struct Entity{
  ID id;
  int x;
  int y;
  int xPrev;
  int yPrev;
  float progress01;
  Behaviour behaviour;

  int strength;

};


struct Position{
  int x;
  int y;
};

enum class Direction{
  RIGHT,
  LEFT,
  UP,
  DOWN
};

bool IsMoving(Entity* entity);

bool HasBehaviour(Entity* entity, Behaviour flags);
void InitializeBaseBehaviour(Entity* entity);
void SetBehaviour(Entity* entity, Behaviour flags);
void AddBehaviour(Entity* entity, Behaviour flags);
void RemoveBehaviour(Entity* entity, Behaviour flags);
