// SPDX-License-Identifier: GPL-3.0-or-later

// Connection
#include <sys/socket.h>

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

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

void loop(int sock, int lobby_size) {
    // Initalisation
    // Conect clients
    struct sockaddr *clientSFD = malloc(lobby_size * sizeof(struct sockaddr));
    int *clients = malloc(sizeof(int)*lobby_size);
    
    int i = 0;
    while (i < lobby_size) {
        socklen_t clientlen = sizeof(clientSFD[i]);
        clients[i] = accept(sock, &clientSFD[i], &clientlen);
        if (clients[i] == -1) {
            fprintf(stderr, "Failed to accept connection: error %d\n", errno);
        } else {
            i++;
        }
    }
    
    // Initalise game state

    // Loop
    //for (;;) {
    //}
    
    // Cleanup
    for (int i = 0; i < lobby_size; i++)
        close(clients[i]);
    free(clients); 
    free(clientSFD);
}
