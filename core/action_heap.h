#pragma once

#include <pthread.h>
#include <stdint.h>

struct action {
  uint64_t time;
};

struct action_heap {
  uint32_t size, cap;
  uint64_t min_time;
  struct action *buckets;
};

struct action_input_queue {
  uint32_t begin, end, size;
  const uint32_t cap;
  pthread_mutex_t mutex;
  struct action *data;
};

extern void push_action_queue(struct action_input_queue *,
                              const struct action *);
extern struct action pop_action_queue(struct action_input_queue *);

extern struct action_heap init_action_heap(uint32_t);
extern void del_action_heap(struct action_heap *);

extern struct action peek_action(const struct action_heap *);

extern void push_action(struct action_heap *, const struct action *);
extern struct action pull_action(struct action_heap *);
