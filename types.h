#pragma once
#include <stdint.h>

struct {
  uint8_t floor;
  uint8_t roof;
  uint8_t obstacle;
  int8_t temperature;
} typedef terrain_t;

struct {
  terrain_t **terrain;
  uint8_t width;
  uint8_t height;
} typedef map_t;

struct {
  uint16_t hp;
  uint16_t stamina;
  int16_t x;
  int16_t y;
  uint16_t type;
} typedef entity_t;

struct {
  uint8_t data[256];
  uint8_t in;
  uint8_t out;
} typedef input_queue_t;

enum window_type_e : uint8_t {
  // default, does nothing
  // used when the window is made to contain subwindows
  WINDOW_TYPE_NONE = 0,

  // make to print map from a buffer and things like that
  WINDOW_TYPE_MAP,

  // have choices as char* + a pointer as a selector
  WINDOW_TYPE_MENU,

  // read inputs from raw char buffer
  WINDOW_TYPE_RAW,

} typedef window_type_e;

struct WINDOW typedef WINDOW;

enum input_code_e : uint8_t {

  CORE_NONE = 0x00,
  CORE_UP = 0x01,
  CORE_DOWN = 0x02,
  CORE_LEFT = 0x03,
  CORE_RIGHT = 0x04,
  CORE_SELECT,

  CORE_EXIT,

};

typedef void (*WindowInputCallback)(WINDOW *const, enum input_code_e const,
                                    void *data);
