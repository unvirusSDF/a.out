#pragma once

#include <stdint.h>

char *lx2str(uint64_t);
uint64_t catn_x64(char *restrict b, uint64_t cap, uint64_t n);
