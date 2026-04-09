#include "core/action_heap.h"
#include <stdio.h>

#define A(a)                                                                   \
  (struct action) { .time = (a) }

int main() {
  struct action_heap a = init_action_heap(10);
  push_action(&a, &A(10));
  push_action(&a, &A(8));
  push_action(&a, &A(9));
  push_action(&a, &A(4));
  push_action(&a, &A(5));
  push_action(&a, &A(7));
  pull_action(&a);

  for (uint32_t i = 0; i < a.size; i++)
    printf("%d, ", a.buckets[i]);
  putchar('\n');
  del_action_heap(&a);
  return 0;
}
