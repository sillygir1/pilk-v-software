#include "main_screen.h"

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

void update_charge(Main_menu *main_menu) {
  char text[64];
  float voltage = read_voltage(main_menu->adc_fd);
  char symbol[4];
  if (voltage > 4.1) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_FULL);
  } else if (voltage > 3.9) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_3);
  } else if (voltage > 3.7) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_2);
  } else if (voltage > 3.5) {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_1);
  } else {
    snprintf(symbol, 4, "%s", LV_SYMBOL_BATTERY_EMPTY);
  }
  snprintf(text, 64, "%.01fV %s", voltage, symbol);
  lv_label_set_text(main_menu->battery_icon, text);
}

static void menu_buttons(const char *button_text) {
  if (strcmp(button_text, main_section[0]) == 0) {
    printf("Launching pm3 client...\n");
    // Clean screen and launch proxmark3 client
  } else if (strcmp(button_text, main_section[1]) == 0) {
    printf("Opening apps view...\n");
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
  Main_menu *main_menu = lv_event_get_user_data(e);

  if (code == LV_EVENT_CLICKED || code == LV_EVENT_PRESSED) {
    // LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
    // printf("Clicked: %s\n", lv_list_get_btn_text(main_menu->list1, obj));
    const char *button_text = lv_list_get_btn_text(main_menu->list1, obj);
    menu_buttons(button_text);
  }
}

void menu_ui(Main_menu *main_menu) {
  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  main_menu->battery_icon = lv_label_create(lv_scr_act());

  lv_label_set_text(main_menu->battery_icon, "0\% \xEF\x89\x82");
  lv_obj_set_align(main_menu->battery_icon, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_style_pad_right(main_menu->battery_icon, 5, LV_PART_MAIN);

  lv_obj_set_y(main_menu->battery_icon, 5);

  lv_obj_t *mode_label = lv_label_create(lv_scr_act());
  lv_label_set_text(mode_label, "USB RNDIS");
  lv_obj_set_align(mode_label, LV_ALIGN_TOP_LEFT);
  lv_obj_set_style_pad_left(mode_label, 5, LV_PART_MAIN);

  lv_obj_set_y(mode_label, 5);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_width(&style, 240);
  lv_style_set_height(&style, 295);
  lv_style_set_y(&style, 25);

  lv_obj_t *list1 = main_menu->list1;
  list1 = lv_list_create(lv_scr_act());
  lv_obj_add_style(list1, &style, LV_PART_MAIN);

  lv_obj_t *btn;
  for (uint8_t i = 0; i < MAIN_SECTION_SIZE; i++) {
    btn = lv_list_add_btn(list1, main_section_icons[i], main_section[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, main_menu);
  }
  lv_list_add_text(list1, "Settings");
  for (uint8_t i = 0; i < SETTINGS_SECTION_SIZE; i++) {
    btn =
        lv_list_add_btn(list1, settings_section_icons[i], settings_section[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, main_menu);
  }
  lv_list_add_text(list1, "Power");
  for (uint8_t i = 0; i < POWER_SECTION_SIZE; i++) {
    btn = lv_list_add_btn(list1, power_section_icons[i], power_section[i]);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, main_menu);
  }
}