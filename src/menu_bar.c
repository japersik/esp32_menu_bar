#include "japersik/esp32_menu_bar/menu_bar.h"

#include <stdio.h>

#include "esp_heap_caps.h"
#include "japersik/esp32_menu_bar/items.h"

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
    default:
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
	printf("Executing action: %s\n", item->title);
	item->value.action_value.action(item->value.action_value.action_ctx);
      }
      menu_bar_back(self);
      break;
    default:
      self->stack_position -= 1;
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

void displayMenu(MenuBar* control) {
  MenuItem* item = _menu_bar_get_head_item(control);

  if (item->type == MENU_TYPE_ACTION) {
    printf("%s?\n", item->title);
    printf("%sYes\n", item->value.action_value.selected ? "->" : "  ");
    printf("%sNo\n\n", item->value.action_value.selected ? "  " : "->");
    return;
  }

  bool selected = item->type != MENU_TYPE_SUBMENU;

  if (selected) {
    item = control->menu_stack[control->stack_position - 1];
  }
  printf(">> %s <<\n", item->title);
  MenuValueSubmenu menu = item->value.submenu;
  for (int i = 0; i < menu.size; i++) {
    MenuItem current = menu.array[i];
    if (i == menu.position) {
      if (current.inactive) {
	printf("- ");
      } else {
	printf("->");
      }
    } else {
      printf("  ");
    }
    printf("%s", current.title);
    if (current.type == MENU_TYPE_INT) {
      printf(": %d", *current.value.int_value.value);
    } else if (current.type == MENU_TYPE_BOOL) {
      printf(": %s", *current.value.bool_value.value ? "ON" : "OFF");
    }
    if (selected && i == menu.position) {
      printf("<-");
    }
    printf("\n");
  }
  printf("\n");
}
