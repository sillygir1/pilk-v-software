#include "encoder.h"

bool running;

static void sig_handler(int _) { running = false; }

int main(void) {
  EncoderData *data = calloc(1, sizeof(*data));
  data->running = &running;
  signal(SIGINT, sig_handler);

  if (encoder_grab(data) != 0) {
    printf("Test: encoder grab failure\n");
    return 1;
  }

  printf("Test: encoder grabbed successfully\n");
  Input input;
  struct timespec rem, req = {0, 200 * 1000 * 1000};
  while (*data->running) {
    if (data->inputQueue[INPUT_DEVICE_ENCODER]->QueueSize > 0) {
      input = input_queue_read(data->inputQueue[INPUT_DEVICE_ENCODER]);
      printf("Test: encoder %d\n", input.key);
    }
    if (data->inputQueue[INPUT_DEVICE_BUTTONS]->QueueSize > 0) {
      input = input_queue_read(data->inputQueue[INPUT_DEVICE_BUTTONS]);
      printf("Test: buttons %d %s\n", input.key, input.type ? "up" : "down");
    }
    nanosleep(&req, &rem);
  }

  printf("\nTest: releasing encoder\n");
  encoder_release(data);
  free(data);
  printf("Test: exit\n");
  return 0;
}
