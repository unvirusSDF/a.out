#pragma once

#include <stdint.h>

enum input_codes : uint8_t {

  CORE_NONE  = 0x00,
  CORE_UP    = 0x01,
  CORE_DOWN  = 0x02,
  CORE_LEFT  = 0x03,
  CORE_RIGHT = 0x04,

  CORE_EXIT  = 0x05,

};
