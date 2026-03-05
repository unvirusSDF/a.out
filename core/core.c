#include "../header.h"
#include "../input_codes.h"

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
  uint8_t inputs_c;
  uint8_t *inputs = next_input(&inputs_c);
  if (inputs) {
    DEBUG(fprintf(stderr, "inputs are : ");
          for (uint8_t i = 0; i < inputs_c; i++)
              fprintf(stderr, "%hhu ", inputs[i]);
          fprintf(stderr, "\n"); free(inputs););

    for (uint8_t i = 0; i < inputs_c; i++)
      switch (inputs[i]) {
      case CORE_NONE:
        break;

      case CORE_UP:
        /*if (entities &&
            map.terrain[entities->y - 1][entities->x].obstacle == ' ') {
          entities->y--;
        }*/
        break;

      case CORE_DOWN:
        /*if (entities &&
            map.terrain[entities->y + 1][entities->x].obstacle == ' ') {
          entities->y++;
        }*/
        break;

      case CORE_LEFT:
        /*if (entities &&
            map.terrain[entities->y][entities->x - 1].obstacle == ' ') {
          entities->x--;
        }*/
        break;

      case CORE_RIGHT:
        /*if (entities &&
            map.terrain[entities->y][entities->x + 1].obstacle == ' ') {
          entities->x++;
        }*/
        break;

      case CORE_EXIT:
        quit();
        break;

      default:
        break;
      }
    return 1;
  } else
    return 0;
}

uint8_t *next_input(uint8_t *n) {
  const uint8_t out = input_queue.out;
  const uint8_t in = input_queue.in;
  input_queue.out = in;

  const uint8_t size = in - out;
  fflush(stderr);
  if (size) {
    uint8_t *ret = malloc(size * sizeof(uint8_t));
    if (!ret)
      return NULL;
    for (uint8_t i = 0; i < size; i++) {
      ret[i] = input_queue.data[i + out];
    }
    *n = size;
    return ret;
  } else
    return NULL;
}
