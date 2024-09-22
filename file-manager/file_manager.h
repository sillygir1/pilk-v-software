#pragma once

#include "battery.h"
#include "encoder.h"
#include "storage.h"
#include "view_manager.h"
#include "view_manager_dialog.h"
#include <sys/time.h>

#define PATH_MAXLEN 128

typedef enum {
  PASTE_NONE,
  PASTE_COPY,
  PASTE_MOVE,

  PASTE_MAX,
} PasteMode;

typedef struct {
  int file_type;
  char *filename;
  char dir[PATH_MAXLEN];
  uint16_t prev_view;
  bool leaving;
  void (*event_handler)(lv_event_t *e);
  char *paste_buff;
  PasteMode paste_mode;
} FileManagerData;

void file_manager_update_list();

void file_manager_init(void *_view_manager, void *ctx);

void file_manager_exit();

lv_obj_t *file_manager_glue_stick();
