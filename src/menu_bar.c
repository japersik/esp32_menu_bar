#include "japersik/esp32_menu_bar/menu_bar.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_heap_caps.h"
#include "japersik/esp32_menu_bar/items.h"

struct MenuBar {
  size_t stack_position;
  size_t stack_size;
  MenuItem* menu_stack[];
};

MenuBar* menu_bar_new(MenuItem* root, size_t max_stack_size) {
  size_t num_items = 10;
  if (max_stack_size != 0) {
    num_items = max_stack_size;
  }

  MenuBar* bar = calloc(1, sizeof(MenuBar) + sizeof(MenuItem*) * num_items);
  if (!bar) {
    return NULL;
  }

  bar->stack_size = num_items;
  bar->menu_stack[0] = root;

  return bar;
}

void menu_bar_free(MenuBar* self) {
  if (self == NULL) {
    return;
  }
  free(self);
}

MenuItem* _menu_bar_get_head_item(MenuBar* self) { return self->menu_stack[self->stack_position]; };

void menu_bar_adjust_value(MenuBar* self, int delta) {
  MenuItem* item = _menu_bar_get_head_item(self);
  if (!item) {
    return;
  }

  switch (item->type) {
    case MENU_TYPE_INT: {
      int* value = item->value.int_value.value;
      if (*value + delta < item->value.int_value.minValue) {
	*value = item->value.int_value.minValue;
      } else if (*value + delta > item->value.int_value.maxValue) {
	*value = item->value.int_value.maxValue;
      } else {
	*value += delta;
      }
      break;
    }
    case MENU_TYPE_SUBMENU:
      if (item->value.submenu.position + delta >= item->value.submenu.size) {
	item->value.submenu.position = item->value.submenu.size - 1;
      } else if (item->value.submenu.position + delta < 0) {
	item->value.submenu.position = 0;
      } else {
	item->value.submenu.position += delta;
      }
      break;
    case MENU_TYPE_BOOL:
      *item->value.bool_value.value = !(*item->value.bool_value.value);
      break;

    case MENU_TYPE_ACTION:
      item->value.action_value.selected = !(item->value.action_value.selected);
      break;
  }
}

void menu_bar_select(MenuBar* self) {
  MenuItem* item = _menu_bar_get_head_item(self);
  if (!item) {
    return;
  }
  switch (item->type) {
    case MENU_TYPE_SUBMENU: {
      MenuItem* next_item = &(item->value.submenu.array[item->value.submenu.position]);
      if (next_item->inactive) {
	return;
      }
      self->stack_position += 1;
      self->menu_stack[self->stack_position] = next_item;
      break;
    }
    case MENU_TYPE_ACTION:
      if (item->value.action_value.selected) {
	item->value.action_value.action(item->value.action_value.action_ctx);
      }
      menu_bar_back(self);
      break;
    case MENU_TYPE_INT:
      menu_bar_back(self);
      break;
    case MENU_TYPE_BOOL:
      menu_bar_back(self);
      break;
  }
}

void menu_bar_back(MenuBar* self) {
  if (self->stack_position == 0) {
    return;
  }
  self->menu_stack[self->stack_position] = NULL;
  self->stack_position--;
}

MenuItem** menu_bar_get_stack(MenuBar* self, size_t* size) {
  if (self == NULL || size == NULL) {
    return NULL;
  }

  *size = self->stack_position + 1;
  return self->menu_stack;
}

