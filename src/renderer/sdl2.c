#include "beam.h"
#include "log.h"
#include "node.h"
#include "node_visitor.h"
#include "renderer.h"
#include "renderer/sdl2.h"
#include <SDL2/SDL.h>
#include <stdio.h>

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define SDL_WIN_FLAGS ( \
    SDL_WINDOW_SHOWN | \
    SDL_WINDOW_RESIZABLE | \
    SDL_WINDOW_ALLOW_HIGHDPI)


static visit_status_t visit_child(node_t *node, void *payload) {
  SDL_Renderer *renderer = (SDL_Renderer *)payload;
  switch (node->type) {
    case BeamTypeBox:

      SDL_SetRenderDrawColor(renderer, 0xff, 0xcc, 0x00, 0xff);
      SDL_Rect r = {
        .x = 100,
        .y = 100,
        .w = get_width(node),
        .h = get_height(node)
      };

      // Draw a rectangle
      SDL_RenderFillRect(renderer, &r);
      break;
    default:
      log_info("FOUND UNKNOWN NODE TYPE: %d", node->type);
      break;
  }

  return VISIT_SUCCESS;
}

static visit_status_t create_window_visitor(node_t *win, void *payload) {
  sdl2_context_t *p = (sdl2_context_t *)payload;

  if (win->type != BeamTypeWindow) {
    log_info("not a window");
    return VISIT_SUCCESS;
  }

  s32_t width = get_width(win);
  if (width == 0) {
    width = SCREEN_WIDTH;
  }

  s32_t height = get_height(win);
  if (height == 0) {
    height = SCREEN_HEIGHT;
  }
  char *title = get_title(win);
  if (strlen(title) == 0) {
    title = "Default Title";
  }

  size_t index = p->window_index++;
  SDL_Window *window = p->windows[index];
  // SDL_Renderer *renderer = p->renderers[index];

  // Initialize SDL
  // if (SDL_Init(SDL_INIT_VIDEO) < 0) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    log_info("SDL could not initialize! SDL_Error: %s", SDL_GetError());
  } else if (window == NULL) {
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WIN_FLAGS);

    if (window == NULL) {
      log_fatal("Window could not be created! SDL_Error: %s", SDL_GetError());
      return VISIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    p->renderers[index] = renderer;
  }

  return VISIT_SUCCESS;
}


static visit_status_t render_window_visitor(node_t *win, void *payload) {
  sdl2_context_t *p = (sdl2_context_t *)payload;

  size_t index = p->window_index++;
  // SDL_Window *window = p->windows[index];
  SDL_Renderer *renderer = p->renderers[index];

  // Clear the screen with the window background
  SDL_SetRenderDrawColor(renderer, 0x16, 0x16, 0x16, 0xff);
  SDL_RenderClear(renderer);
  each_child(win, visit_child, renderer);
  SDL_RenderPresent(renderer);

  return VISIT_SUCCESS;
}

beam_signal_t *beam_signals_gather(beam_surface_t *s) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_KEYDOWN:
        SDL_Log("User just pressed down a key!");
        break;  
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
          SDL_Log("User closed the window!");
          s->should_close = true;
        }
        break;
      default:
        break;
    }
  }
  // log_info("beam_signals_gather with type: %d", s->type);
  return NULL;
}

beam_surface_t *beam_create_surface(beam_surface_type t) {
  beam_surface_t *s = calloc(sizeof(beam_surface_t), 1);
  if (s == NULL) {
    log_info("failed to allocate beam_surface_t");
    return NULL;
  }

  s->platform = calloc(sizeof(sdl2_context_t), 1);
  s->type = t;
  return s;
}

s32_t beam_window_should_close(beam_surface_t *s) {
  if (s != NULL && s->should_close) {
    return 1;
  }

  return 0;
}

void beam_surface_free(beam_surface_t *s) {
  sdl2_context_t *p = s->platform;

  if (p != NULL) {
    // Destroy window
    for (size_t i = 0; i < p->window_count; i++) {
      SDL_DestroyWindow(p->windows[i]);
    }
  }

  // Quit SDL subsystems
  SDL_Quit();

  free(p);
  free(s);
}

s32_t beam_render(beam_surface_t *surface, beam_signal_t *signals,
                 node_t *root) {
  if (signals == NULL) {
    // beam_signal_t *s = *signals;
    // log_info("and signals: %d", s->type);
  }

  if (surface == NULL) {
    log_info("SURFACE is null");
    return -1;
  }

  sdl2_context_t *p = surface->platform;
  if (p == NULL) {
    log_info("PLATFORM IS NULL");
    return -1;
  }

  if (root != NULL) {
    p->window_count = get_child_count(root);
  
    if (p->windows == NULL && p->window_count > 0) {
      p->windows = (SDL_Window**)malloc(p->window_count * sizeof(SDL_Window *));
      // p->surfaces = (SDL_Surface**)malloc(p->window_count * sizeof(SDL_Surface *));
      p->renderers = (SDL_Renderer**)malloc(p->window_count * sizeof(SDL_Renderer *));
      each_child(root, create_window_visitor, p);
      p->window_index = 0;
    }

    each_child(root, render_window_visitor, p);
    p->window_index = 0;
  } else {
    log_info("ROOT IS NULL?");
    printf("SDSDF\n");
  }

  SDL_Delay(3);

  return BeamSuccess;
}
