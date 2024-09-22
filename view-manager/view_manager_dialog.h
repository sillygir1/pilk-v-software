#pragma once

#include "view_manager.h"
#include "view_manager_extra.h"

typedef struct {
  char *title;
  char **options;
  void **icons;
  uint8_t options_num;
  void (*cb)(char *);
} ViewManagerDialog;

void view_manager_dialog(void *_view_manager, ViewManagerDialog *_vm_dialog);
