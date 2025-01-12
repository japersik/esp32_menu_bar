#ifndef _JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H
#define _JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "items.h"

typedef struct MenuBar {
  MenuItem *main_item;
  MenuItem *menu_stack[100];
  size_t stack_position;
} MenuBar;

void menu_bar_select(MenuBar *);
void menu_bar_back(MenuBar *);
void menu_bar_adjust_value(MenuBar *, int delta);
void displayMenu(MenuBar *control);
#endif	// !_JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H
