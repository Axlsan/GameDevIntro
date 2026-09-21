#pragma once

#include "entity.h"
#include "levels.h"
#include <cstdint>

enum class CMD_TYPE : uint8_t{
  NONE = 0,
  MOVE = 1,
  ROTATE = 2
};

struct Command{
  CMD_TYPE type = CMD_TYPE::NONE;
  uint32_t timestamp;
};

struct MoveCommand : Command{
  Entity* entity;
  int xDir;
  int yDir;

  MoveCommand(Entity* entity, int xDir, int yDir){
    this->entity = entity;
    this->xDir = xDir;
    this->yDir = yDir;
    type = CMD_TYPE::MOVE;
  }
};

struct RotateCommand : Command{
  Entity* entity;
  Direction from;
  Direction to;

  RotateCommand(Entity* entity, Direction from, Direction to){
    this->entity = entity;
    this->from = from;
    this->to = to;
    type = CMD_TYPE::ROTATE;
  }
};

union AnyCommand{
  Command command;
  MoveCommand move;
  RotateCommand rotate;

  AnyCommand(MoveCommand mv){
    move = mv;
  }

  AnyCommand(RotateCommand ro){
    rotate = ro;
  }
};

struct CommandBuffer{
  AnyCommand* allCommands;
  int capacity;
  int index;
  int head;
};


void Push(CommandBuffer* buffer, AnyCommand cmd, LevelData* lvl, uint32_t timestamp);
void Undo(CommandBuffer* buffer);
void Redo(CommandBuffer* buffer, LevelData* lvl);
