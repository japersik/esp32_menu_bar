
#include "japersik/esp32_menu_bar/display.h"

#include <stddef.h>
#include <stdio.h>

#include "japersik/esp32_menu_bar/items.h"
#include "japersik/esp32_menu_bar/menu_bar.h"

void _display_item(MenuItem* item, bool current, bool selected) {
  if (!item) {
    return;
  }
  if (current) {
    if (item->inactive) {
      printf("- ");
    } else {
      printf("->");
    }
  } else {
    printf("  ");
  }
  printf("%s", item->title);
  switch (item->type) {
    break;
    case MENU_TYPE_INT:
      printf(": %d", *item->value.int_value.value);
      break;
    case MENU_TYPE_BOOL:
      printf(": %s", *item->value.bool_value.value ? "ON" : "OFF");
      break;
    case MENU_TYPE_ACTION:
    case MENU_TYPE_SUBMENU:
      break;
  }
  if (selected) {
    printf("<-");
  }
  printf("\n");
};

void _display(MenuItem** items_stask, size_t stack_size) {
  if (stack_size == 0) {
    return;
  }
  MenuItem* item = items_stask[stack_size - 1];

  if (item->type == MENU_TYPE_ACTION) {
    printf("%s?\n", item->title);
    printf("%sYes\n", item->value.action_value.selected ? "->" : "  ");
    printf("%sNo\n\n", item->value.action_value.selected ? "  " : "->");
    return;
  }

  bool selected_menu = false;
  if (item->type != MENU_TYPE_SUBMENU) {
    if (stack_size > 1 && items_stask[stack_size - 2]->type == MENU_TYPE_SUBMENU) {
      selected_menu = true;
      item = items_stask[stack_size - 2];
    } else {
      _display_item(item, false, false);
      return;
    }
  }
  printf(">> %s <<\n", item->title);
  MenuValueSubmenu menu = item->value.submenu;
  for (int i = 0; i < menu.size; i++) {
    MenuItem current = menu.array[i];
    bool is_current = i == menu.position;
    _display_item(&current, is_current, is_current && selected_menu);
  }

  printf("\n");
};

void menu_bar_display_print(MenuBar* self) {
  size_t stack_size = 0;
  MenuItem** items = menu_bar_get_stack(self, &stack_size);
  _display(items, stack_size);
}
