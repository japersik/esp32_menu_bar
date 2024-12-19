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
#include "japersik/esp32_menu_bar/menu_bar.h"
#include "portmacro.h"
#include "string.h"

#define TXD_PIN 16
#define RXD_PIN 17

#define RX_BUF_SIZE 2048
int brightness = 50;  // Пример настройки
bool wifiEnabled = true;

void* doAction(void* params) {
  printf("Doing action!\n");
  return 0;
}

typedef struct MenuControl {
  MenuItem* menuStack[100];
  size_t stackPosition;
} MenuControl;

void displayMenu(MenuControl* control) {
  MenuItem* item = control->menuStack[control->stackPosition];

  bool selected = item->type != MENU_TYPE_SUBMENU;

  if (selected) {
    item = control->menuStack[control->stackPosition - 1];
  }
  printf(">> %s <<\n", item->title);
  MenuItemMenuList menu = item->value.submenu;
  for (int i = 0; i < menu.size; i++) {
    MenuItem current = menu.array[i];
    if (i == menu.position) {
      printf("->");
    } else {
      printf("  ");
    }
    printf("%s", current.title);

    // Отображение значения для числовых и логических параметров
    if (current.type == MENU_TYPE_INT) {
      printf(": %d", *current.value.intValue.value);
    } else if (current.type == MENU_TYPE_BOOL) {
      printf(": %s", *current.value.boolValue.value ? "ON" : "OFF");
    }
    if (selected && i == menu.position) {
      printf("<-");
    }
    printf("\n");
  }
  printf("\n");
}

void deselectItem(MenuControl* control) {
  if (control->stackPosition > 0)
    control->stackPosition--;
}
void selectItem(MenuControl* control) {
  MenuItem* item = control->menuStack[control->stackPosition];
  switch (item->type) {
    case MENU_TYPE_SUBMENU:
      control->stackPosition += 1;
      control->menuStack[control->stackPosition] = &(item->value.submenu.array[item->value.submenu.position]);
      break;
    case MENU_TYPE_ACTION:
      if (item->value.action) {
	printf("Executing action: %s\n", item->title);
	item->value.action(0);	// Вызов функции действия
	deselectItem(control);
      }
      break;
    default:
      control->stackPosition -= 1;
      break;
  }
}
void adjustValue(MenuControl* self, int delta) {
  MenuItem* item = self->menuStack[self->stackPosition];

  switch (item->type) {
    case MENU_TYPE_INT: {
      int* value = item->value.intValue.value;
      *value += delta;
      if (*value < item->value.intValue.minValue) {
	*value = item->value.intValue.minValue;
      }
      if (*value > item->value.intValue.maxValue) {
	*value = item->value.intValue.maxValue;
      }
      break;
    }
    case MENU_TYPE_SUBMENU:
      item->value.submenu.position += delta;
      if (item->value.submenu.position >= item->value.submenu.size) {
	item->value.submenu.position = item->value.submenu.size - 1;
      }
      if (item->value.submenu.position < 0) {
	item->value.submenu.position = 0;
      }
      break;
    case MENU_TYPE_BOOL: {
      if (delta != 0) {	 // delta можно игнорировать для булевых значений
	*item->value.boolValue.value = !(*item->value.boolValue.value);
      }
      break;
    }
    default:
      break;
  }
}

void pressedCallback(void* param, ButtonEvent enevt) {
  MenuControl* menuControl = (MenuControl*)(param);
  if (enevt & BUTTON_RELEASE_SHORT_CLICK) {
    selectItem(menuControl);
    displayMenu(menuControl);
  } else if (enevt & BUTTON_HOLD) {
    deselectItem(menuControl);
    displayMenu(menuControl);
  }
}

void encoderCallback(EncoderCallbackEvent event, void* param) {
  MenuControl* menuControl = (MenuControl*)(param);
  adjustValue(menuControl, event.delta);
  displayMenu(menuControl);
}

void app_main(void) {
  MenuItem submenuElems[] = {
      {"Info", MENU_TYPE_ACTION, .value.action = doAction},
      {"Test", MENU_TYPE_ACTION, .value.action = doAction},
      {"Do action", MENU_TYPE_ACTION, .value.action = doAction},
      {"Do action 2", MENU_TYPE_ACTION, .value.action = doAction},
  };
  int test = 1;
  MenuItem mainMenuElems[] = {
      {"Int value (0,100)", MENU_TYPE_INT, .value.intValue = {&brightness, 0, 100, false}},
      {"Int value (-10,10)", MENU_TYPE_INT, .value.intValue = {&test, -10, 10, false}},
      {"Bool value", MENU_TYPE_BOOL, .value.boolValue = {&wifiEnabled}},
      {"Action", MENU_TYPE_ACTION, .value.action = doAction},
      {"Submenu", MENU_TYPE_SUBMENU, .value.submenu = {.position = 0, .size = 4, submenuElems}},
  };

  MenuItem menu = {
      "Main menu",
      MENU_TYPE_SUBMENU,
      .value.submenu = {.size = 5, .position = 0, .array = mainMenuElems},
  };

  MenuControl menuControl = {{&menu}, 0};

  gpio_install_isr_service(0);

  VirtButtonConfig bCfg = {.event_callback = pressedCallback, .event_callback_params = &menuControl, .inverse = 1};
  VirtButton* b = button_new(&bCfg);
  ButtonObserver* obs = button_observer_new();

  PcntEncoder* encoder = pcnt_encoder_new(21, 22);
  pcnt_encoder_set_callback(encoder, encoderCallback, &menuControl);

  button_observer_add_button(obs, b, 23);
  for (;;) {
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}
