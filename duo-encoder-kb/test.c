#include "encoder.h"

bool running;

static void sig_handler(int _)
{
  running = false;
}

int main(void)
{
  Data* data = malloc(sizeof(*data));
  data->running = &running;
  signal(SIGINT, sig_handler);

  if (encoder_grab(data) != 0)
  {
    printf("Test: encoder grab failure\n");
    return 1;
  }

  printf("Test: encoder grabbed successfully\n");
  Input input;
  struct timespec rem, req = {0, 200 * 1000 * 1000};
  while (*data->running)
  {
    if (data->inputQueueSize > 0)
    {
      input = input_queue_read(data);
      printf("Test: pressed %d key, %s press\n", input.key,
             input.type ? "long" : "short");
    }
    nanosleep(&req, &rem);
  }

  printf("\nTest: releasing encoder\n");
  encoder_release(data);
  free(data);
  printf("Test: exit\n");
  return 0;
}
