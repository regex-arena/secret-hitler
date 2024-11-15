// SPDX-License-Identifier: GPL-3.0-or-later

#include <curses.h>
#include <form.h>
#include <signal.h>
#include <stdlib.h>

static void finish();

void render(int sock) {

    signal(SIGINT, finish);
    WINDOW *main_screen = initscr();
    keypad(stdscr, true);
    nonl();
    cbreak();

    if (has_colors()) {
        start_color();

        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(1, COLOR_RED, COLOR_BLACK);
    }

    FIELD *nfield = new_field(8, 16
                              );

    for (;;) {
        int c = getchar();

        refresh();
    }
}

void finish() {
    endwin();
    exit(0);
}
