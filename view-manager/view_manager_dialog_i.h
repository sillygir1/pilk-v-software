#pragma once
#include "view_manager_dialog.h"

#define DIALOGS_MAX 5

typedef struct {
  lv_group_t *input_group[2];
  lv_indev_t *enc_indev[2];
  lv_obj_t *list;
} ViewManagerDialogI;

typedef struct {
  ViewManagerDialogI *dialog[DIALOGS_MAX];
  uint8_t count;
} ViewManagerDialogStack;
