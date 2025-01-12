#ifndef _JAPERSIK_ESP32_MENU_BAR_ITEMS_H
#define _JAPERSIK_ESP32_MENU_BAR_ITEMS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct MenuItem MenuItem;

typedef struct MenuValueInt {
  int *value;
  int minValue;
  int maxValue;
  bool overflowWrap;
} MenuValueInt;

typedef struct MenuValueBool {
  bool *value;
} MenuValueBool;

typedef struct MenuValueSubmenu {
  int position;
  int size;
  MenuItem *array;
} MenuValueSubmenu;

typedef struct MenuValueAction {
  void *(*action)(void *user_ctx);
  void *action_ctx;
  bool with_approve;
} MenuValueAction;

typedef enum { MENU_TYPE_SUBMENU, MENU_TYPE_INT, MENU_TYPE_BOOL, MENU_TYPE_ACTION } MenuValueType;

typedef union {
  MenuValueSubmenu submenu;
  MenuValueBool bool_value;
  MenuValueInt int_value;
  MenuValueAction action_value;
} MenuValue;

struct MenuItem {
  const char *title;
  MenuValueType type;
  MenuValue value;
};

#endif	// !_JAPERSIK_ESP32_MENU_BAR_ITEMS_H
