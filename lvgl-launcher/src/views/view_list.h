#pragma once

typedef enum {
  VIEW_NONE = 0,
  VIEW_MAIN_MENU,
  VIEW_APPS,
  
  VIEW_COUNT, // Always last element
} ViewList;

void views_init(void *_view_manager);
