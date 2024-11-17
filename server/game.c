// SPDX-License-Identifier: GCC-BY-NC-SA-4.0

// Connection
#include <sys/socket.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

const int NAME_LENGHT=8;

struct GameState {
    // Deck contains 17 cards so uint16_t has too few bits
    uint32_t pdeck;
    char dfullness;
    char hitler;
    char facist[4];
    char players;
    char fpolicy;
    char lpolicy;
};

uint32_t popdeck() {
    uint32_t deck = 0;
    int deckseed[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    // Fisher-Yates shuffle
    for (int i = 0; i < 15; i++) {
        int j = rand()%(i+1);
        int jval = deckseed[j];
        deckseed[j] = deckseed[i];
        deckseed[i] = jval;
    }
    for (int i = 0; i < 11; i++) {
        deck = deck | 1<<(deckseed[i]);
    }
    printf("%b\n", deck);

    return deck;
}

void loop(int *clients, int lobby_size) {
    struct GameState state;
    state.pdeck = popdeck();

}
