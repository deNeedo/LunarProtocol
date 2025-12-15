#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/frame.h"

#define HUB_INBOX   "/hub_inbox"
#define ROVER_OUTBOX "/rover_outbox"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./rover <id>\n");
        return 1;
    }

    int rover_id = atoi(argv[1]);

    mqd_t hub_in, rover_out;
    
    hub_in = mq_open(HUB_INBOX, O_WRONLY);
    rover_out = mq_open(ROVER_OUTBOX, O_RDONLY);

    if (hub_in == -1 || rover_out == -1) {
        perror("mq_open");
        exit(1);
    }

    printf("[ROVER %d] Running...\n", rover_id);

    while (1) {
        Frame f = {0};
        f.version = 1;
        f.sender_id = rover_id;
        f.target_id = 0;
        f.msg_type = 0;
        snprintf((char *)f.payload, PAYLOAD_SIZE, "Hello from rover %d", rover_id);

        mq_send(hub_in, (char *)&f, sizeof(Frame), 0);
        printf("[ROVER %d] Sent message\n", rover_id);

        Frame incoming = {0};
        ssize_t bytes = mq_receive(rover_out, (char *)&incoming, sizeof(Frame), NULL);

        if (bytes > 0) {
            printf("[ROVER %d] Received HUB broadcast: %s\n",
                rover_id, incoming.payload);
        }

        sleep(2);
    }

    return 0;
}
