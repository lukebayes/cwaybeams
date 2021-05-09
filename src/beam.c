#include "beam.h"
#include <stdlib.h>
#include <stdio.h>

beam_surface_t *beam_create_surface(beam_surface_type t) {
  beam_surface_t *s = calloc(sizeof(beam_surface_t), 1);
  s->type = t;
  return s;
}

void beam_render(beam_surface_t *s, beam_signal_t *signals, node_t *node) {
  printf("beam_render with: %d\n", s->type);
  printf("and node: %d\n", node->type);
  printf("and signals: %d\n", signals->type);
}

beam_signal_t **beam_signals_gather(beam_surface_t *s) {
  printf("beam_signals_gather with: %d\n", s->type);
  return NULL;
}
