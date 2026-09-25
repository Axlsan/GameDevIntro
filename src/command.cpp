

#include "command.h"
#include "entity.h"
#include "levels.h"

void Execute(AnyCommand cmd, LevelData* lvl, CommandBuffer* commandBuffer, bool fromRedo = false){
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;

    case CMD_TYPE::MOVE:{
      MoveCommand* mv = &cmd.move;
      mv->entity->xPrev = mv->entity->x;
      mv->entity->yPrev = mv->entity->y;
      mv->entity->x += mv->xDir;
      mv->entity->y += mv->yDir;
      if(fromRedo){
        mv->entity->progress01 = 1;
      }
      

      PostMove(mv->entity, lvl, commandBuffer);
      
      break;
    }

    case CMD_TYPE::ROTATE:{
      RotateCommand* rotate = &cmd.rotate;
      if(!HasBehaviour(rotate->entity, CAN_ROTATE)){
        break;
      }

      PreRotation(rotate->entity, lvl, commandBuffer, rotate->from, rotate->to);
      
      rotate->entity->facing = rotate->to;

      PostRotation(rotate->entity, lvl, commandBuffer, rotate->from, rotate->to);
    }

    case CMD_TYPE::MODIFY_BEHAVIOUR:{
      ModifyBehaviourCommand* modify = &cmd.modify;
      if(modify->mode == ModifyBehaviourCommand::ADD){
        AddBehaviour(modify->entity, modify->flag);
      }
      else{
        RemoveBehaviour(modify->entity, modify->flag);
      }
      break;
    }

    case CMD_TYPE::ADD:{
      AddCommand* add = &cmd.add;
      AddEntity(add->id, add->x, add->y, lvl);
      break;
    }

    case CMD_TYPE::REMOVE:{
      RemoveCommand* remove = &cmd.remove;
      RemoveEntity(remove->x, remove->y, lvl);
      break;
    }
  }
}

void Push(CommandBuffer* buffer, AnyCommand cmd, LevelData* lvl){
  assert(cmd.command.type != CMD_TYPE::NONE);
  
  buffer->allCommands[buffer->index] = cmd;

  buffer->allCommands[buffer->index].command.timestamp = buffer->timestamp;
  
  buffer->index++;
  buffer->head = buffer->index;
  Execute(cmd, lvl, buffer);
}

void Undo(CommandBuffer* buffer, LevelData* lvl){
  if (buffer->index == 0){
    return;
  }
  buffer->index--;

  AnyCommand cmd = buffer->allCommands[buffer->index];
  uint32_t timestamp = cmd.command.timestamp;
  switch(cmd.command.type){
    case CMD_TYPE::NONE:
      break;

    case CMD_TYPE::MOVE:{
      MoveCommand mv = cmd.move;
      mv.entity->x -= mv.xDir;
      mv.entity->y -= mv.yDir;
      mv.entity->progress01 = 1; // anim
      break;
    }
    case CMD_TYPE::ROTATE:{
      RotateCommand rotate = cmd.rotate;
      if(!HasBehaviour(rotate.entity, Behaviour::CAN_ROTATE)){
        break;
      }
      rotate.entity->facing = rotate.from;
      // break;
    }
    case CMD_TYPE::MODIFY_BEHAVIOUR:{
      ModifyBehaviourCommand modify = cmd.modify;
      if(modify.mode == ModifyBehaviourCommand::ADD){
        AddBehaviour(modify.entity, modify.flag);
      }
      else{
        RemoveBehaviour(modify.entity, modify.flag);
      }
      break;
    }

    case CMD_TYPE::ADD:{
      AddCommand* add = &cmd.add;
      RemoveEntity(add->x, add->y, lvl);
      break;
    }

    case CMD_TYPE::REMOVE:{
      RemoveCommand* remove = &cmd.remove;
      AddEntity(remove->storedID, remove->x, remove->y, lvl);
      Entity* entity = GetEntity(lvl, remove->x, remove->y);
      SetBehaviour(entity, remove->storedBehaviour);
      break;
    }
  }
  if(buffer->index > 0){
    if(buffer->allCommands[buffer->index - 1].command.timestamp == timestamp){
      Undo(buffer, lvl);
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

  Execute(cmd, lvl, buffer, true);

  buffer->index++;

  int timestamp = cmd.command.timestamp;

  if(buffer->index != buffer->head){
    AnyCommand nextCommand = buffer->allCommands[buffer->index];
    if(nextCommand.command.timestamp == timestamp){
      Redo(buffer, lvl);
    }
  }
}
 
