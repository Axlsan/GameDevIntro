#pragma once
#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(10);

constexpr int FPS = 240;
const double FRAME_TIME_MS = 1000.0 / FPS;

const int SCREEN_WIDTH = 1300;
const int SCREEN_HEIGHT = 800;
const int UPSCALE_FACTOR = 3;
const int CELL_SIZE_PX = 16 * UPSCALE_FACTOR;

const float MOVE_SPEED = 6.0;
