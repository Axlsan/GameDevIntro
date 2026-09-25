#include "dev_gui.h"
#include "SDL3/SDL_video.h"
#include "gameState.h"
#include "command.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "imgui/imgui_internal.h"
#include "levelEditor.h"
#include <cmath>
#include <string>

using namespace std;
void DEV::Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
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

void DrawHistory(CommandBuffer* buffer, LevelData* lvl){
  int sliderPos = buffer->index;
  if(ImGui::SliderInt("history", &sliderPos, 0, buffer->head)){
    while(buffer->index > sliderPos){
      Undo(buffer, lvl);
    }
    while(buffer->index < sliderPos){
      Redo(buffer, lvl);
    }
  }
}


void DrawFPS(float dt){
  ImGui::Text("FPS: %0.f", 1 / dt);
}

void DrawColor(){
  // Edit a color stored as 4 floats
//  ImGui::ColorEdit4("Color", );
}

void DrawSamples(){
  // Generate samples and plot them
  float samples[100];
  for (int n = 0; n < 100; n++)
      samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
  ImGui::PlotLines("Samples", samples, 100);
  
}


void DrawMouseClick(GameData* data){
  ImGui::Text("Mouse: %d", data->input.mouseCurrent );
}




void DEV::Draw(GameData* data, SDL_Renderer* renderer){
  ImGui::Begin("Dev Tools");

  // CODE HERE
  DrawImGuiArenaUsage(data->arenaImages, "images");
  DrawImGuiArenaUsage(data->arenaLevels, "levels");
  DrawImGuiArenaUsage(data->arenaCommands, "commands");
  DrawImGuiArenaUsage(data->arenaEntities, "entities");
  DrawHistory(data->commandBuffer, data->GetCurrentLevel());
  DrawFPS(*data->dt);

  DrawColor();
  DrawSamples();
  DrawMouseClick(data);

  if(data->editLevel){
    EDITOR::DrawObjectPanel(&data->editorData, data->spriteBuffer);
    EDITOR::DrawPreview(&data->editorData, &data->input, renderer, data->GetCurrentLevel(), &data->camera, data->spriteBuffer);
  }
  
  // END CODE
  ImGui::End();
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  
}

