// Connection
#include <sys/socket.h>

#include <stdint.h>
#include <stdlib.h>

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
    int *clients = malloc(lobby_size * sizeof(int));
    int i = 0;
    while (i < lobby_size) {
        listen(sock, lobby_size);
        accept(sock, , );
        i++;
    }
    
    // Initalise game state

    // Loop
    for (;;) {
    }
    
    // Cleanup
    free(clients); 
}
