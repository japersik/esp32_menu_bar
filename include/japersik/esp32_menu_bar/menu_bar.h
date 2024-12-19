#ifndef _JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H
#define _JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct MenuItem MenuItem;

typedef enum {
  MENU_TYPE_SUBMENU,
  MENU_TYPE_INT,
  MENU_TYPE_BOOL,
  MENU_TYPE_ACTION
} MenuItemType;

typedef struct MenuItemIntValue {
  int *value;
  int minValue;
  int maxValue;
  bool overflowWrap;
} MenuItemIntValue;

typedef struct MenuItemBoolValue {
  bool *value;
} MenuItemBoolValue;

typedef struct MenuItemMenuList {
  size_t position;
  size_t size;
  MenuItem *array;
} MenuItemMenuList;

struct MenuItem {
  const char *title;
  MenuItemType type;
  union {
    MenuItemMenuList submenu;
    MenuItemBoolValue boolValue;
    MenuItemIntValue intValue;
    void *(*action)(void *);
  } value;
};

#endif // !_JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H
