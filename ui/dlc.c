#include "./decl.h"

void *input_listener(void *) {
  halfdelay(2);
  int c;
  while (listening_inputs) {
    if ((c = getch()) != ERR)
      if (keybinds[c]) {
        input_queue->data[input_queue->in++] = keybinds[c];
        // fprintf(stderr, "input detected (%c) (in : %hhu)\n", c,
        // input_queue->in); fflush(stderr);
      }
  }
}

#define COLOR_X_LIST()                                                         \
  X(BLACK) X(RED) X(GREEN) X(YELLOW) X(BLUE) X(MAGENTA) X(CYAN) X(WHITE)
#define X(color) #color,
const char *colors_names[8] = {COLOR_X_LIST()};
#undef X

// init the colors with dark magic
void init_color_map(void) {
  for (uint8_t i = 0; i < 0b01000000; i++) {
    init_pair(i, i & 0b111, i >> 3);
    fprintf(stderr, "foreground %7s background %7s mapped to %i\n",
            colors_names[i & 0b111], colors_names[i >> 3], i);
  }
}

// colors goes from 0-7 -> w/ background and no color duplicates 64 colors pairs
// (NOTE: no bright backgournd)
//  attributes : 0b 00 000 000
//
//  texture plan:
//   - map floor to backround color
//   - map obstacle to char // use plain char but it could heave texture
//   conflict? and could be seem arbitrarry values OR
//       array to map, however this would be 256B wich is not that much
//
//   - roof = dark -> no roof bright
//
//   - /!\ char color and other attributes not used yet
//
//
//   - MAYBE NEED TO MAP PACKED ATTRIBUTES TO THE TERRAIN INSTEAD OF LETTING
//   THAT TO THE API
//

// try to follow the plan described above
int map_texture_to_terrain(const terrain_t *t) {
  int attributes = {};
  if (!t->roof)
    attributes |= A_BOLD;

  attributes |= COLOR_PAIR(((t->floor & 0b111) << 3) |
                           COLOR_WHITE); // print in white with a background

  return attributes;
}
