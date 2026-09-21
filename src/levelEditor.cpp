#include "levelEditor.h"
#include "camera.h"
#include "imgui/imgui.h"
#include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"

namespace EDITOR{
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer){
    ImGui::Begin("objects");
    ImVec2 size = {32, 32};

    if(ImGui::ImageButton("Ground", (ImTextureID)GetSpriteFromID(ID::GROUND, spriteBuffer)->texture, size)){
      editor->objectToPlaceID = ID::GROUND;
    }

    ImGui::SameLine();
    if(ImGui::ImageButton("Wall", (ImTextureID)GetSpriteFromID(ID::WALL, spriteBuffer)->texture, size)){
      editor->objectToPlaceID = ID::WALL;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Rock", (ImTextureID)GetSpriteFromID(ID::ROCK, spriteBuffer)->texture, size)){
      editor->objectToPlaceID = ID::ROCK;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Demon", (ImTextureID)GetSpriteFromID(ID::DEMON, spriteBuffer)->texture, size)){
      editor->objectToPlaceID = ID::DEMON;
    }
    ImGui::SameLine();
    if(ImGui::ImageButton("Medusa", (ImTextureID)GetSpriteFromID(ID::MEDUSA, spriteBuffer)->texture, size)){
      editor->objectToPlaceID = ID::MEDUSA;
    }

    ImGui::End();
  }


  void PlaceObject(const int x, const int y, Editor* editor, LevelData* lvl){
    if(editor->objectToPlaceID == ID::GROUND || editor->objectToPlaceID == ID::WALL){
      lvl->cells[y * lvl->w + x] = (int)editor->objectToPlaceID;
    }
    else{
      AddEntity(editor->objectToPlaceID, x, y, lvl);
    }
  }

  void DrawPreview(Editor* editor, Input* input, SDL_Renderer* renderer, LevelData* lvl, Camera* camera, Sprite* spriteBuffer){
    int x;
    int y;
    camera::WorldToGrid(input->mouseX, input->mouseY, &x, &y, lvl);
    Sprite* preview = GetSpriteFromID(editor->objectToPlaceID, spriteBuffer);
    
    if(preview != nullptr){
      RenderSprite_Grid(preview, lvl, renderer, camera, x, y, 1, 0.5);
    }
  }

  void Update(Editor* editor, Input* input, LevelData* level){
    if(MousePressed(input, MouseButtons::LEFT)){
      if(camera::IsPointInGrid(input->mouseX, input->mouseY, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouseX, input->mouseY, &x, &y, level);
        PlaceObject(x, y, editor, level);
      }
    }
    else if(MousePressed(input, MouseButtons::RIGHT)){
      if(camera::IsPointInGrid(input->mouseX, input->mouseY, level)){
        int x;
        int y;
        camera::WorldToGrid(input->mouseX, input->mouseY, &x, &y, level);
        RemoveEntity(x, y, level);
      }
    }
  }
}

