#include "battery.h"

#ifndef PLUGIN
bool running;
#endif

int adc_init() {
  if ((system("lsmod | grep -q \"cv181x_saradc\"") == 0)) {
    printf("SARADC module already loaded.\n");
  } else {
    system("insmod $(find / -name \"cv181x_saradc.ko\" 2>/dev/null)");
    printf("SARADC module loaded.\n");
  }
  int fd = open("/sys/class/cvi-saradc/cvi-saradc0/device/cv_saradc",
                O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    printf("Error at opening ADC!\n");
    return -1;
  }

  char adc_channel = ADC_CHANNEL;
  write(fd, &adc_channel, 1);

  return fd;
}

float read_voltage(int fd) {
  char buffer[8];
  int len = 0;
  int adc_value = 0;
  for (int i = 0; i < sizeof(buffer); i++) {
    buffer[i] = 0;
  }
  lseek(fd, 0, SEEK_SET);
  len = read(fd, buffer, sizeof(buffer) - 1);
  if (len != 0) {
    adc_value = atoi(buffer);
#ifndef PLUGIN
    printf("ADC%c value is %d\n", ADC_CHANNEL, adc_value);
#endif
  }
  float voltage =
      ((float)adc_value / 4096 * 3.3) * (float)(R5 + R6) / (float)R6;
#ifndef PLUGIN
  printf("%.02f volts\n", voltage);
#endif
  return voltage;
}

#ifndef PLUGIN

static void sig_handler(int _) { running = false; }

int main() {
  signal(SIGINT, sig_handler);

  running = true;

  int fd = adc_init();
  if (fd < 0) {
    return 1;
  }

  while (running) {
    read_voltage(fd);
    sleep(1);
  }

  printf("\n");
  close(fd);
  return 0;
}

#endif