#include "../header.h"

#include <stdio.h>
#include <stdlib.h>

volatile uint8_t quit_c = 0;
uint8_t quit() { return ++quit_c; }

uint8_t must_quit() {
  if (quit_c) {
    quit_c--;
    return quit_c + 1;
  }
  return 0;
}

uint8_t run_frame(void) {
  if (something_happend_counter) {
    something_happend_counter = 0;
    return 1;
  }
  return 0;
}
