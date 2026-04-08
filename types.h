#pragma once
#include <stdint.h>

// made to be passed as buffer wich reinterpret them as
// struct{
//   uint16_t selector, choices_n;
//   const char*const* choices
// }
struct menu_t {
  uint32_t selector, choices_n;
  char const *const *choices;
  void (**choices_ppfn)(void);
} typedef menu_t;

// a variety of flag to decribe a entity
enum entity_traits : uint16_t {
  ENT_TRAIT_none = 0x0000,
  ENT_TRAIT_moveable = 0x0001,
  ENT_TRAIT_targetable = 0x0002,
  ENT_TRAIT_crossable = 0x0004,
  ENT_TRAIT_visible = 0x0008,
  ENT_TRAIT_playable = 0x0010,
};

struct {
  int16_t hp;
  int16_t stamina;

  int16_t x;
  int16_t y;
  enum entity_traits traits;
} typedef entity_t;

enum terrain_traits : uint8_t {
  TERRAIN_TRAIT_none = 0x00,
  TERRAIN_TRAIT_crossable = 0x01,
};

struct {
  uint8_t floor;
  uint8_t roof;
  uint8_t obstacle;
  enum terrain_traits traits;
} typedef terrain_t;

struct {
  // may become use if map_view is intended to be in use
  // uint16_t maxh, maxw, y, x;
  uint16_t height, width;
  terrain_t **terrain;
  uint16_t cursor_y, cursor_x;
  entity_t *pentities;
  uint16_t entities_n;
} typedef map_t;

enum window_type : uint8_t {
  // default, does nothing
  // used when the window is made to contain subwindows
  WINDOW_TYPE_NONE = 0,

  // make to print map from a buffer and things like that
  WINDOW_TYPE_MAP,

  // have choices as char* + a pointer as a selector
  WINDOW_TYPE_MENU,

  // read inputs from raw char buffer
  WINDOW_TYPE_RAW,

};

uintptr_t typedef WINDOW;

enum input_code_e : uint8_t {

  CORE_NONE = 0x00,
  CORE_UP,
  CORE_DOWN,
  CORE_LEFT,
  CORE_RIGHT,

  CORE_CURSOR_UP,
  CORE_CURSOR_DOWN,
  CORE_CURSOR_LEFT,
  CORE_CURSOR_RIGHT,

  CORE_SELECT,

  CORE_EXIT,

};

typedef void (*window_input_callback_pfn)(WINDOW const, enum input_code_e const,
                                          void *data);

struct window_create_info_t {
  uint32_t height, width;
  enum window_type type;
  union {
    const void *pbuffer;
    // struct{
    //   const WINDOW **ppsubwindows;
    //   uint16_t subwindow_count;
    // };
  };
  window_input_callback_pfn pfn_input_callback;
} typedef window_create_info_t;
