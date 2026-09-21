

#include "command.h"
#include "levels.h"
#include <random>

void Execute(AnyCommand cmd, LevelData* lvl, bool fromRedo = false){
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;
    case CMD_TYPE::MOVE:
      MoveCommand mv = cmd.move;
        mv.entity->xPrev = mv.entity->x;
        mv.entity->yPrev = mv.entity->y;
        mv.entity->x += mv.xDir;
        mv.entity->y += mv.yDir;
        if(fromRedo){
          mv.entity->progress01 = 1;
        }
        break;
  }
}

void Push(CommandBuffer* buffer, AnyCommand cmd, LevelData* lvl, uint32_t timestamp){
  assert(cmd.command.type != CMD_TYPE::NONE);
  
  buffer->allCommands[buffer->index] = cmd;

  buffer->allCommands[buffer->index].command.timestamp = timestamp;
  
  buffer->index++;
  buffer->head = buffer->index;
  Execute(cmd, lvl);
}

void Undo(CommandBuffer* buffer){
  if (buffer->index == 0){
    return;
  }
  buffer->index--;

  AnyCommand cmd = buffer->allCommands[buffer->index];
  uint32_t timestamp = cmd.command.timestamp;
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;

    case CMD_TYPE::MOVE:
      MoveCommand mv = cmd.move;
      mv.entity->x -= mv.xDir;
      mv.entity->y -= mv.yDir;
      mv.entity->progress01 = 1; // anim
      break;
  }
  if(buffer->index > 0){
    if(buffer->allCommands[buffer->index - 1].command.timestamp == timestamp){
      Undo(buffer);
    }
  }
}

void Redo(CommandBuffer* buffer, LevelData* lvl){
  AnyCommand cmd = buffer->allCommands[buffer->index];
  if(cmd.command.type == CMD_TYPE::NONE){
    return;
  }

  if(buffer->index == buffer->head){
    return;
  }

  Execute(cmd, lvl, true);

  buffer->index++;

  int timestamp = cmd.command.timestamp;

  if(buffer->index != buffer->head){
    AnyCommand nextCommand = buffer->allCommands[buffer->index];
    if(nextCommand.command.timestamp == timestamp){
      Redo(buffer, lvl);
    }
  }
}
 
