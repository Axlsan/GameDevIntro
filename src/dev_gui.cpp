#include "dev_gui.h"
#include "gameState.h"
#include "command.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "imgui/imgui_internal.h"
#include <cmath>
#include <string>

using namespace std;
void DEV::Initialize(SDL_Window* window, SDL_Renderer *renderer){
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  ImGuiIO& io = ImGui::GetIO();
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  io.DisplaySize = ImVec2((float)w, (float)h);
}

void DEV::ProcessEvents(SDL_Event *event){
  ImGui_ImplSDL3_ProcessEvent(event);
}

void DEV::PreDraw(ImGuiContext* savedContext){
  if(ImGui::GetCurrentContext() == nullptr){
    ImGui::SetCurrentContext(savedContext);
  }
  
  ImGui::NewFrame();
}


void DrawImGuiArenaUsage(Memory::Arena* arena, std::string nameOfArena){
  float fraction = (float)arena->used / (float)arena->size;

  string barText = nameOfArena;
  barText += " " + to_string(arena->used);
  barText += " / " + to_string(arena->size);

  ImGui::ProgressBar(fraction, ImVec2(-1,0), barText.c_str());
  
}

void DrawHistory(CommandBuffer* buffer){
  int sliderPos = buffer->index;
  if(ImGui::SliderInt("history", &sliderPos, 0, buffer->head)){
    while(buffer->index > sliderPos){
      Undo(buffer);
    }
    while(buffer->index < sliderPos){
      Redo(buffer);
    }
  }
}


void DrawFPS(float dt){
  ImGui::Text("FPS: %0.f", 1 / dt);
}


void DEV::Draw(GameData* data, SDL_Renderer* renderer){
  ImGui::Begin("Dev Tools");

  // CODE HERE
  DrawImGuiArenaUsage(data->arenaImages, "images");
  DrawImGuiArenaUsage(data->arenaLevels, "levels");
  DrawImGuiArenaUsage(data->arenaCommands, "commands");
  DrawImGuiArenaUsage(data->arenaEntities, "entities");
  DrawHistory(data->commandBuffer);
  DrawFPS(*data->dt);
  
  // END CODE
  ImGui::End();
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  
}

