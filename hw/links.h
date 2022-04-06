#pragma once

#include "v4l2.h"

typedef struct buffer_s buffer_t;
typedef struct buffer_list_s buffer_list_t;

typedef void (*link_on_buffer)(buffer_t *buf);
typedef bool (*link_check_streaming)();

typedef struct link_s {
  struct buffer_list_s *source; // capture_list
  struct buffer_list_s *sinks[10];
  struct {
    link_on_buffer on_buffer;
    link_check_streaming check_streaming;
  } callbacks;
} link_t;

int links_init(link_t *all_links);
int links_step(link_t *all_links, int timeout);
int links_loop(link_t *all_links, bool *running);