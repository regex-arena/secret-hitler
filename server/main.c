// Connection
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <stdlib.h>

// IO
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {
    // Initilises hints to 0
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *result, *rp;
    int sock;

    hints.ai_family = AF_UNSPEC; // Can use both IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM; // Stream prefered over DGRAM due to reliability
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if (getaddrinfo(NULL, "4848", &hints, &result) != 0) {
        fprintf(stderr, "Unable to get address info: %s", strerror(errno));
        return 1;
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1)
            continue;
        if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0)
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
