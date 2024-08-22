#pragma once

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/font/lv_symbol_def.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "battery.h"
#include "encoder.h"

typedef struct {
  bool *running;
  Data *enc_data;
  lv_group_t *input_group;
  lv_obj_t *battery_icon;
  lv_obj_t *mode_label;
  int adc_fd;
} Main_menu;

void draw_status_bar();
