#include "main_screen_view.h"
#include "view_list.h"

#define MENU_ITEMS_CNT 9
#define MENU_LABELS_CNT 2
char *menu_items[MENU_ITEMS_CNT] = {
    "Proxmark3 client", "Apps",  "Settins", "UART config", "USB config",
    "System settings",  "Power", "Reboot",  "Power off"};
const char *menu_icons[MENU_ITEMS_CNT] = {
    LV_SYMBOL_DRIVE,   LV_SYMBOL_DIRECTORY, LV_SYMBOL_DUMMY,
    LV_SYMBOL_SHUFFLE, LV_SYMBOL_USB,       LV_SYMBOL_SETTINGS,
    LV_SYMBOL_DUMMY,   LV_SYMBOL_REFRESH,   LV_SYMBOL_POWER};
int menu_labels[MENU_LABELS_CNT] = {2, 6};

static lv_obj_t *list;
static ViewManager *view_manager;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(list, obj);
    if (strcmp(button_text, "Proxmark3 client") == 0) {
      menu_exit();
      launch_app("proxmark3 -p /dev/ttyS1 -b 57600");
      menu_init(view_manager, NULL);
    } else if (strcmp(button_text, "Apps") == 0) {
      view_manager_switch_view(view_manager, VIEW_APPS, NULL);
    } else if (strcmp(button_text, "UART config") == 0) {
      printf("Opening uart config...\n");
      // Do the thing
    } else if (strcmp(button_text, "USB config") == 0) {
      printf("Opening usb config...\n");
      // Do the thing
    } else if (strcmp(button_text, "System settings") == 0) {
      printf("Opening system settings...\n");
      // Do the thing
    } else if (strcmp(button_text, "Reboot") == 0) {
      lv_obj_clean(lv_scr_act());
      system("reboot");
    } else if (strcmp(button_text, "Power off") == 0) {
      lv_obj_clean(lv_scr_act());
      system("poweroff");
    }
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      // Do nothing on the main screen
      // printf("Back button pressed!\n");
    }
  }
}

void menu_init(void *_view_manager, void *ctx) {
  view_manager = _view_manager;
  set_mode_text("Pilk-V Duo");

  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  ViewManagerList vm_list = {menu_items,      (const void **)menu_icons,
                             MENU_ITEMS_CNT,  menu_labels,
                             MENU_LABELS_CNT, event_handler};
  list = view_manager_list_init(view_manager, &vm_list);
}

void menu_exit() { lv_obj_del(list); }