#include <stdlib.h>

#include "action_heap.h"

#ifndef SWAP
#define SWAP(a, b)                                                             \
  do {                                                                         \
    typeof(a) tmp = a;                                                         \
    a = b, b = tmp;                                                            \
  } while (0)
#endif

struct action peek_action(const struct action_heap *h) { return *h->buckets; }

struct action_heap init_action_heap(uint32_t cap) {
  return (struct action_heap){
      .size = 0,
      .cap = cap,
      .buckets = malloc(cap * sizeof *(struct action_heap){}.buckets),
  };
}

void del_action_heap(struct action_heap *h) {
  free(h->buckets);
  *h = (struct action_heap){};
}

void push_action(struct action_heap *h, const struct action *a) {
  if (h->size >= h->cap)
    h->buckets = realloc(h->buckets, (h->cap *= 2) * sizeof *h->buckets);

  h->buckets[h->size] = *a;
  for (uint32_t i = h->size++; i != 0; i = (i - 1) / 2)
    if (h->buckets[i].time < h->buckets[(i - 1) / 2].time)
      SWAP(h->buckets[i], h->buckets[(i - 1) / 2]);
  h->min_time = h->buckets->time;
}

struct action pull_action(struct action_heap *h) {
  if (!h->size)
    return (struct action){};
  struct action min = peek_action(h);
  h->buckets[0] = h->buckets[--h->size];
  uint32_t smallest = 0;
  for (;;) {
    uint32_t l = 2 * smallest + 1, r = 2 * smallest + 2;
    if (l < h->size && h->buckets[l].time < h->buckets[smallest].time) {
      SWAP(h->buckets[l], h->buckets[smallest]);
      smallest = l;
      continue;
    }
    if (r < h->size && h->buckets[r].time < h->buckets[smallest].time) {
      SWAP(h->buckets[r], h->buckets[smallest]);
      smallest = l;
      continue;
    }
    break;
  }
  return min;
}

void push_action_queue(struct action_input_queue *q, const struct action *a) {
  pthread_mutex_lock(&q->mutex);
  if (q->size >= q->cap) {
    pthread_mutex_unlock(&q->mutex);
    return;
  }
  q->data[q->end++] = *a;
  if (q->end >= q->cap)
    q->end = 0;
  q->size++;
  pthread_mutex_unlock(&q->mutex);
}

struct action pop_action_queue(struct action_input_queue *q) {
  pthread_mutex_lock(&q->mutex);
  if (!q->size) {
    pthread_mutex_unlock(&q->mutex);
    return (struct action){};
  }
  struct action a = q->data[q->begin++];
  if (q->begin >= q->cap)
    q->begin = 0;
  q->size--;
  pthread_mutex_unlock(&q->mutex);
  return a;
}
