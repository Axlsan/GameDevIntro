#include "entity.h"
#include "command.h"
#include "levels.h"

bool IsMoving(Entity* entity){
  return entity->x != entity->xPrev || entity->y != entity->yPrev;
}


bool HasBehaviour(Entity* entity, Behaviour flags){
  return (entity->behaviour & flags) == flags;
}

void InitializeBaseBehaviour(Entity* entity){
  //assert(entity->id != ID::NONE);
    switch (entity->id) {

    default:
        SetBehaviour(entity, NONE);
      break;

    case ID::MEDUSA:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | CAN_ROTATE | IS_PLAYER | RESPOND_TO_INPUT));
      AddBehaviour(entity, Behaviour::JUMPS);
      entity->strength = 1;
      break;
      
    case ID::DEMON:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
      AddBehaviour(entity, Behaviour::JUMPS);
      entity->strength = 1;
      break;
      
    case ID::GOLEM:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT | CAN_ROTATE));
      AddBehaviour(entity, Behaviour::UNPUSHABLE);
      entity->strength = 999;
      break;

    case ID::SIREN:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
      entity->strength = 0;
      break;
      
    case ID::ROCK:
      SetBehaviour(entity, (Behaviour)CAN_MOVE);
      break;

  }
}

void SetBehaviour(Entity* entity, Behaviour flags){
  entity->behaviour = flags;
}

void AddBehaviour(Entity* entity, Behaviour flags){
      entity->behaviour = (Behaviour)(entity->behaviour | flags);
}

void RemoveBehaviour(Entity* entity, Behaviour flags){
      entity->behaviour = (Behaviour)(entity->behaviour & ~flags);
}


void PostMove(Entity* entity, LevelData* lvl, CommandBuffer* commandBuffer){
  if(entity->id == ID::MEDUSA){
    Entity* entityLookedAt = RaycastFirstEntity(entity->x, entity->y, entity->facing, lvl);
    if(entityLookedAt != nullptr){
      if(!HasBehaviour(entityLookedAt, Behaviour::IS_PETRIFIED)){
        ModifyBehaviourCommand modify(entityLookedAt, Behaviour::IS_PETRIFIED, ModifyBehaviourCommand::ADD);
        Push(commandBuffer, modify, lvl);
      }
    }
  }
  
}

void PostRotation(Entity* entity, LevelData* lvl, CommandBuffer* commandBuffer, Direction from, Direction to){
  if(from == to){
    return;
  }

  if(entity->id == ID::MEDUSA){
    Entity* entityLookedAt = RaycastFirstEntity(entity->x, entity->y, to, lvl);
    if(!HasBehaviour(entityLookedAt, Behaviour::IS_PETRIFIED)){
      ModifyBehaviourCommand modify(entityLookedAt, Behaviour::IS_PETRIFIED, ModifyBehaviourCommand::ADD);
      Push(commandBuffer, modify, lvl);
    }
  }
}

void PreRotation(Entity* entity, LevelData* lvl, CommandBuffer* commandBuffer, Direction from, Direction to){
  if(from == to){
    return;
  }
  
  if(entity->id == ID::MEDUSA){
    Entity* entityPreviouslyLookedAt = RaycastFirstEntity(entity->x, entity->y, from, lvl);
    if(entityPreviouslyLookedAt != nullptr){
      ModifyBehaviourCommand modify(entityPreviouslyLookedAt, Behaviour::IS_PETRIFIED, ModifyBehaviourCommand::REMOVE);
      Push(commandBuffer, modify, lvl);
    }
  }
}
