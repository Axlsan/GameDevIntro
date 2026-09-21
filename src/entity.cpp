#include "entity.h"

bool IsMoving(Entity* entity){
  return entity->x != entity->xPrev || entity->y != entity->yPrev;
}


bool HasBehaviour(Entity* entity, Behaviour flags){
  return (entity->behaviour & flags) == flags;
}

void InitializeBaseBehaviour(Entity* entity){
  assert(entity->id != ID::NONE);
    switch (entity->id) {

    default:
        SetBehaviour(entity, NONE);
      break;

    case ID::MEDUSA:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
      entity->strength = 1;
      break;
      
    case ID::DEMON:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
      entity->strength = 1;
      break;
      
    case ID::GOLEM:
      SetBehaviour(entity, (Behaviour)(CAN_MOVE | IS_PLAYER | RESPOND_TO_INPUT));
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


