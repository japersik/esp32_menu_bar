#ifndef _JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H
#define _JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "items.h"

typedef struct MenuBar MenuBar;

MenuBar *menu_bar_new(MenuItem *root, size_t max_stack_size);
void menu_bar_free(MenuBar *);

// control funcs
void menu_bar_select(MenuBar *);
void menu_bar_back(MenuBar *);
void menu_bar_adjust_value(MenuBar *, int delta);

// for display
MenuItem **menu_bar_get_stack(MenuBar *self, size_t *size);

#endif	// !_JAPERSIK_ESP32_MENU_BAR_MENU_BAR_H
