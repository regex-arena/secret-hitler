// SPDX-License-Identifier: GPL-3.0-or-later

// Connect
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

// Standard
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Output
#include <stdio.h>

int main(int argc, char** argv) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *result, *rp;
    int sock;
    
    hints.ai_family = AF_UNSPEC; // Can use both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // Stream prefered over DGRAM due to reliability
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if (getaddrinfo(argv[1], "4848", &hints, &result) != 0) {
        fprintf(stderr, "Unable to get address info: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1)
            continue;
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(sock);
    }
    freeaddrinfo(result);

    if (rp == NULL) {
        fprintf(stderr, "Unable to bind to socket");
        exit(EXIT_FAILURE);
    }

    return 0;
}
