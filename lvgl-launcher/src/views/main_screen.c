#include "main_screen.h"

static lv_obj_t *main_menu_list;

#define MAIN_SECTION_SIZE 2
#define SETTINGS_SECTION_SIZE 3
#define POWER_SECTION_SIZE 2
const char *main_section[MAIN_SECTION_SIZE] = {"Proxmark3 client", "Apps"};
const char *main_section_icons[MAIN_SECTION_SIZE] = {LV_SYMBOL_DRIVE,
                                                     LV_SYMBOL_DIRECTORY};
const char *settings_section[SETTINGS_SECTION_SIZE] = {
    "UART config", "USB config", "System settings"};
const char *settings_section_icons[SETTINGS_SECTION_SIZE] = {
    LV_SYMBOL_SHUFFLE, LV_SYMBOL_USB, LV_SYMBOL_SETTINGS};
const char *power_section[POWER_SECTION_SIZE] = {"Reboot", "Power off"};
const char *power_section_icons[POWER_SECTION_SIZE] = {LV_SYMBOL_REFRESH,
                                                       LV_SYMBOL_POWER};

static void menu_buttons(char *button_text) {
  if (strcmp(button_text, main_section[0]) == 0) {
    printf("Launching pm3 client...\n");
    launch_client();
    // Clean screen, release encoder and launch proxmark3 client
  } else if (strcmp(button_text, main_section[1]) == 0) {
    printf("Opening apps view...\n");
    lv_obj_del(main_menu_list);
    apps_ui();
    // Clean screen and switch to app menu view
  } else if (strcmp(button_text, settings_section[0]) == 0) {
    printf("Opening uart config...\n");
    // Do the thing
  } else if (strcmp(button_text, settings_section[1]) == 0) {
    printf("Opening usb config...\n");
    // Do the thing
  } else if (strcmp(button_text, settings_section[2]) == 0) {
    printf("Opening system settings...\n");
    // Do the thing
  } else if (strcmp(button_text, power_section[0]) == 0) {
    printf("Rebooting...\n");
    lv_obj_clean(lv_scr_act());
    system("reboot");
  } else if (strcmp(button_text, power_section[1]) == 0) {
    printf("Shutting down...\n");
    lv_obj_clean(lv_scr_act());
    system("poweroff");
  }
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  // Main_menu *main_menu = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED) {
    const char *button_text = lv_list_get_btn_text(main_menu_list, obj);
    menu_buttons(button_text);
  } else if (code == LV_EVENT_KEY) {
    if (lv_indev_get_key(lv_indev_get_act()) == LV_KEY_ESC) {
      // Do nothing on the main screen
      // printf("Back button pressed!\n");
    }
  }
}

void menu_ui() {
  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_width(&style, 240);
  lv_style_set_height(&style, 295);
  lv_style_set_y(&style, 25);

  main_menu_list = lv_list_create(lv_scr_act());
  lv_obj_add_style(main_menu_list, &style, LV_PART_MAIN);

  lv_obj_t *btn;
  for (uint8_t i = 0; i < MAIN_SECTION_SIZE; i++) {
    btn =
        lv_list_add_btn(main_menu_list, main_section_icons[i], main_section[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  }
  lv_list_add_text(main_menu_list, "Settings");
  for (uint8_t i = 0; i < SETTINGS_SECTION_SIZE; i++) {
    btn = lv_list_add_btn(main_menu_list, settings_section_icons[i],
                          settings_section[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  }
  lv_list_add_text(main_menu_list, "Power");
  for (uint8_t i = 0; i < POWER_SECTION_SIZE; i++) {
    btn = lv_list_add_btn(main_menu_list, power_section_icons[i],
                          power_section[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  }
}