#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/projdefs.h"
#include "japersik/esp32_button/gpio_observer.h"
#include "japersik/esp32_button/virt_button.h"
#include "japersik/esp32_encoder/pcnt_encoder.h"
#include "japersik/esp32_encoder/virt_encoder.h"
#include "japersik/esp32_menu_bar/items.h"
#include "japersik/esp32_menu_bar/menu_bar.h"
#include "portmacro.h"
#include "string.h"

int led_delay_on = 50;	 // Пример настройки
int led_delay_off = 50;	 // Пример настройки

bool wifiEnabled = true;

void pressedCallback(ButtonEvent enevt, void* param) {
  MenuBar* menuControl = (MenuBar*)(param);
  if (enevt & BUTTON_RELEASE_SHORT_CLICK) {
    menu_bar_select(menuControl);
    displayMenu(menuControl);
  } else if (enevt & BUTTON_HOLD) {
    menu_bar_back(menuControl);
    displayMenu(menuControl);
  }
}

void encoderCallback(EncoderCallbackEvent event, void* param) {
  MenuBar* menuControl = (MenuBar*)(param);
  menu_bar_adjust_value(menuControl, event.delta);
  displayMenu(menuControl);
}

void* print_text(void* text) {
  printf("printing '%s'\n", (char*)(text));
  return 0;
}

void app_main(void) {
  int t = 0;
  MenuItem submenuElems[] = {
      {"Info", MENU_TYPE_INT, .value.int_value = {&t, -10, 10, false}, true},
      {"Test", MENU_TYPE_INT, .value.int_value = {&t, -10, 10, false}},
      {"Do action", MENU_TYPE_ACTION, .value.action_value = {print_text, "action1", false}, true},
      {"Do action 2", MENU_TYPE_ACTION, .value.action_value = {print_text, "action1", false}},
  };
  MenuItem mainMenuElems[] = {
      {"led on (0,100)", MENU_TYPE_INT, .value.int_value = {&led_delay_on, 0, 100, false}},
      {"led off (0,100)", MENU_TYPE_INT, .value.int_value = {&led_delay_off, 0, 100, false}},
      {"Bool value", MENU_TYPE_BOOL, .value.bool_value = {&wifiEnabled}},
      {"Submenu", MENU_TYPE_SUBMENU, .value.submenu = {.position = 0, .size = 4, submenuElems}},
  };

  MenuItem menu = {
      "Main menu",
      MENU_TYPE_SUBMENU,
      .value.submenu = {.size = 4, .position = 0, .array = mainMenuElems},
  };

  MenuBar menuControl = {{&menu}, 0};

  gpio_install_isr_service(0);

  VirtButtonConfig bCfg = {.event_callback = pressedCallback, .event_callback_ctx = &menuControl, .inverse = 1};
  VirtButton* b = button_new(&bCfg);
  ButtonObserver* obs = button_observer_new();

  PcntEncoder* encoder = pcnt_encoder_new(22, 23);
  pcnt_encoder_set_callback(encoder, encoderCallback, &menuControl);

  button_observer_add_button(obs, b, 21);
  gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
  for (;;) {
    gpio_set_level(GPIO_NUM_18, 1);
    vTaskDelay(led_delay_on * 10 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_18, 0);
    vTaskDelay(led_delay_off * 10 / portTICK_PERIOD_MS);
  }
}
