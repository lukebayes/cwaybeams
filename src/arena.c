#include "arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arena_t {
  size_t size;
  size_t used;
  u8_t *data;
} arena_t;

static arena_t _arena = {0};

s8_t arena_init(size_t size) {
  if (_arena.data != NULL) {
    printf("arena_init: already initialized\n");
    return -1;
  }

  _arena.size = size;
  _arena.used = 0;
  _arena.data = malloc(size);
  if (_arena.data == NULL) {
    printf("arena_new: malloc failed\n");
    return -1;
  }

  return 0;
}

void *arena_malloc(size_t size) {
  if (_arena.used + size > _arena.size) {
    printf("arena_malloc: out of memory\n");
    return NULL;
  }

  void *ptr = _arena.data + _arena.used;
  _arena.used += size;
  return ptr;
}

void *arena_calloc(size_t size, u32_t count) {
  u64_t total = size * count;
  if (total > SIZE_MAX) {
    printf("arena_calloc: size too large\n");
    return NULL;
  }

  void *ptr = arena_malloc(total);
  if (ptr == NULL) {
    return NULL;
  }

  memset(ptr, 0, total);
  return ptr;
}

void arena_reset(void) {
  _arena.used = 0;
}

void arena_free(void) {
  if (_arena.data == NULL) {
    printf("arena_free called, but not initialized\n");
    return;
  }
  free(_arena.data);
  _arena.data = NULL;
}
