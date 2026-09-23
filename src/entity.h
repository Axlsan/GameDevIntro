#pragma once

//#include "command.h"
#include <cassert>
#include <cstdint>

struct CommandBuffer;

enum Behaviour : uint32_t{
  NONE = 0,
  CAN_MOVE = 1 << 0,
  IS_PLAYER = 1 << 1,
  RESPOND_TO_INPUT = 1 << 2,
  IS_PETRIFIED = 1 << 3,
  CAN_ROTATE = 1 << 4,
  UNPUSHABLE = 1 << 5,
  JUMPS = 1 << 6,
  IS_PUSHING = 1 << 7
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


inline Direction DirectionFromXY(int dirX, int dirY){
  assert(dirX * dirY == 0);

  if(dirX == 1) return Direction::RIGHT;
  if(dirX == -1) return Direction::LEFT;
  if(dirY == 1) return Direction::UP;
  else return Direction::DOWN;
}

struct Entity{
  ID id;
  int x;
  int y;
  int xPrev;
  int yPrev;
  float progress01;
  Behaviour behaviour;

  int strength;

  Direction facing;

};

struct LevelData;

bool IsMoving(Entity* entity);

bool HasBehaviour(Entity* entity, Behaviour flags);
void InitializeBaseBehaviour(Entity* entity);
void SetBehaviour(Entity* entity, Behaviour flags);
void AddBehaviour(Entity* entity, Behaviour flags);
void RemoveBehaviour(Entity* entity, Behaviour flags);

void PostMove(Entity* entity, LevelData* lvl, CommandBuffer* commandBuffer);
void PostRotation(Entity* entity, LevelData* lvl, CommandBuffer* commandBuffer, Direction from, Direction to);
void PreRotation(Entity* entity, LevelData* lvl, CommandBuffer* commandBuffer, Direction from, Direction to);
