#pragma once
#include <stdint.h>

typedef struct {
  uint8_t floor;
  uint8_t roof;
  uint8_t obstacle;
  uint8_t temperature;
} terrain_t;

typedef struct {
  terrain_t terrain[64][64];
  uint8_t width;
  uint8_t height;
} map_chunk_t; // at the beginning this was 128*128 but 16kB is a better unit
               // than 65kB

typedef struct {
  uint16_t hp;
  uint16_t stamina;
  int16_t x;
  int16_t y;
  uint16_t type;
} entity_t;

typedef struct {
  uint8_t data[256];
  uint8_t in;
  uint8_t out;
} input_queue_t;
