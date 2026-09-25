#pragma once

#include "entity.h"
#include "levels.h"
#include <cstdint>

enum class CMD_TYPE : uint8_t {
  NONE = 0,
  MOVE = 1,
  ROTATE = 2,
  MODIFY_BEHAVIOUR = 3,
  ADD = 4,
  REMOVE = 5
};

struct Command {
  CMD_TYPE type = CMD_TYPE::NONE;
  uint32_t timestamp;
};

struct MoveCommand : Command {
  Entity *entity;
  int xDir;
  int yDir;

  MoveCommand(Entity *entity, int xDir, int yDir) {
    this->entity = entity;
    this->xDir = xDir;
    this->yDir = yDir;
    type = CMD_TYPE::MOVE;
  }
};

struct RotateCommand : Command {
  Entity *entity;
  Direction from;
  Direction to;

  RotateCommand(Entity *entity, Direction from, Direction to) {
    this->entity = entity;
    this->from = from;
    this->to = to;
    type = CMD_TYPE::ROTATE;
  }
};

struct ModifyBehaviourCommand : Command {
  enum Mode { ADD, REMOVE };

  Entity *entity;
  Behaviour flag;
  Mode mode;
  ModifyBehaviourCommand(Entity *entity, Behaviour flag, Mode mode) {
    this->entity = entity;
    this->flag = flag;
    this->mode = mode;
    type = CMD_TYPE::MODIFY_BEHAVIOUR;
  }
};

struct AddCommand : Command {
  int x;
  int y;
  ID id;

  AddCommand(int x, int y, ID id) {
    this->x = x;
    this->y = y;
    this->id = id;
    type = CMD_TYPE::ADD;
  }
};

struct RemoveCommand : Command {
  int x;
  int y;
  Behaviour storedBehaviour;
  ID storedID;

  RemoveCommand(Entity *entity) {
    x = entity->x;
    y = entity->y;
    storedBehaviour = entity->behaviour;
    storedID = entity->id;
    type = CMD_TYPE::REMOVE;
  }
};

union AnyCommand {
  Command command;
  MoveCommand move;
  RotateCommand rotate;
  ModifyBehaviourCommand modify;
  AddCommand add;
  RemoveCommand remove;

  AnyCommand(MoveCommand mv) { move = mv; }

  AnyCommand(RotateCommand ro) { rotate = ro; }

  AnyCommand(ModifyBehaviourCommand mod) { modify = mod; }

  AnyCommand(AddCommand add) { this->add = add; }

  AnyCommand(RemoveCommand remove) { this->remove = remove; }
};

struct CommandBuffer {
  AnyCommand *allCommands;
  int capacity;
  int index;
  int head;

  uint32_t timestamp;
};

void Push(CommandBuffer *buffer, AnyCommand cmd, LevelData *lvl);
void Undo(CommandBuffer *buffer, LevelData* lvl);
void Redo(CommandBuffer *buffer, LevelData *lvl);
