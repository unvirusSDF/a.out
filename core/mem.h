#pragma once

#include "../types.h"
#include <stdio.h>

void *load_menu_from_mem(menu_t *out, FILE *f);
void *load_map_from_mem(map_t *out, FILE *f);
void *save_map_to_mem(map_t const *, FILE *);
