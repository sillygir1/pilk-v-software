#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/font/lv_symbol_def.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "battery.h"
#include "encoder.h"

#define DISP_BUF_SIZE (128 * 1024)

bool running;
Data *enc_data;
static lv_obj_t *list1;
static lv_group_t *input_group;

lv_obj_t *battery_icon;

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
  }
}

static void test_ui();

static void sig_handler(int _) { running = false; }

static void encoder_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);

int main(void) {
  /*LittlevGL init*/
  lv_init();

  /*Linux frame buffer device init*/
  fbdev_init();

  /*A small buffer for LittlevGL to draw the screen's content*/
  static lv_color_t buf[DISP_BUF_SIZE];

  /*Initialize a descriptor for the buffer*/
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

  /*Initialize and register a display driver*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf;
  disp_drv.flush_cb = fbdev_flush;
  disp_drv.hor_res = 240;
  disp_drv.ver_res = 320;
  lv_disp_drv_register(&disp_drv);

  /*Initialize and register an input driver*/
  static lv_indev_drv_t encoder_drv;
  lv_indev_drv_init(&encoder_drv);
  encoder_drv.type = LV_INDEV_TYPE_ENCODER;
  encoder_drv.read_cb = encoder_cb;
  lv_indev_t *encoder_indev = lv_indev_drv_register(&encoder_drv);

  input_group = lv_group_create();
  lv_group_set_default(input_group);
  lv_indev_set_group(encoder_indev, input_group);

  test_ui();

  signal(SIGINT, sig_handler);
  enc_data = malloc(sizeof(*enc_data));
  enc_data->running = &running;
  encoder_grab(enc_data);
  struct timespec rem, req = {0, 100 * 1000 * 1000};
  Input input;

  int adc_fd = adc_init();
  if (adc_fd < 0)
    printf("Adc init issue");

  /*Handle LitlevGL tasks (tickless mode)*/
  while (running) {
    lv_timer_handler();

    char text[64];
    snprintf(text, 64, "%.01fV", read_voltage(adc_fd));
    lv_label_set_text(battery_icon, text);

    nanosleep(&req, &rem);
  }
  encoder_release(enc_data);
  close(adc_fd);
  free(enc_data);
  return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void) {
  static uint64_t start_ms = 0;
  if (start_ms == 0) {
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);
    start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
  }

  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  uint64_t now_ms;
  now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

  uint32_t time_ms = now_ms - start_ms;
  return time_ms;
}

static void test_ui() {
  // Disabling scrollbar
  lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

  battery_icon = lv_label_create(lv_scr_act());

  lv_label_set_text(battery_icon, "0\% \xEF\x89\x82");
  lv_obj_set_align(battery_icon, LV_ALIGN_TOP_RIGHT);
  lv_obj_set_style_pad_right(battery_icon, 5, LV_PART_MAIN);

  lv_obj_set_y(battery_icon, 5);

  lv_obj_t *mode_label = lv_label_create(lv_scr_act());
  lv_label_set_text(mode_label, "Standalone mode");
  lv_obj_set_align(mode_label, LV_ALIGN_TOP_LEFT);
  lv_obj_set_style_pad_left(mode_label, 5, LV_PART_MAIN);

  lv_obj_set_y(mode_label, 5);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_width(&style, 240);
  lv_style_set_height(&style, 295);
  lv_style_set_y(&style, 25);

  list1 = lv_list_create(lv_scr_act());
  lv_obj_add_style(list1, &style, LV_PART_MAIN);

  /*Add buttons to the list*/
  lv_obj_t *btn;

  // lv_list_add_text(list1, "Main");
  btn = lv_list_add_btn(list1, LV_SYMBOL_DRIVE, "Proxmark3 client");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
  btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Apps");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

  lv_list_add_text(list1, "Settings");
  btn = lv_list_add_btn(list1, LV_SYMBOL_BLUETOOTH, "UART config");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
  btn = lv_list_add_btn(list1, LV_SYMBOL_USB, "USB config");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
  btn = lv_list_add_btn(list1, LV_SYMBOL_SETTINGS, "System settings");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

  lv_list_add_text(list1, "Power");
  btn = lv_list_add_btn(list1, LV_SYMBOL_REFRESH, "Reboot");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
  btn = lv_list_add_btn(list1, LV_SYMBOL_POWER, "Shut down");
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
}

static void encoder_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Input input;
  if (enc_data->inputQueue[INPUT_DEVICE_ENCODER]->QueueSize > 0) {
    input = input_queue_read(enc_data->inputQueue[INPUT_DEVICE_ENCODER]);
    printf("CB: encoder %s\n", input.key ? "down" : "up");
    switch (input.key) {
    case INPUT_KEY_UP:
      data->enc_diff = -1;
      break;
    case INPUT_KEY_DOWN:
      data->enc_diff = 1;
      break;
    default:
      printf("Input reading error\n");
      break;
    }
  }
}
