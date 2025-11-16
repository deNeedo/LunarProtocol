#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include "frame.h"

#define HUB_INBOX   "/hub_inbox"
#define ROVER_OUTBOX "/rover_outbox"

int main() {
    mqd_t hub_in, rover_out;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Frame);
    attr.mq_curmsgs = 0;

    mq_unlink(HUB_INBOX);
    mq_unlink(ROVER_OUTBOX);

    hub_in = mq_open(HUB_INBOX, O_RDONLY | O_CREAT, 0666, &attr);
    rover_out = mq_open(ROVER_OUTBOX, O_WRONLY | O_CREAT, 0666, &attr);

    if (hub_in == -1 || rover_out == -1) {
        perror("mq_open");
        exit(1);
    }

    printf("[HUB] Running...\n");

    while (1) {
        Frame f;
        ssize_t bytes_read = mq_receive(hub_in, (char *)&f, sizeof(Frame), NULL);

        if (bytes_read > 0) {
            printf("[HUB] From Rover %d: CMD=%d  MSG=%s\n",
                f.sender_id, f.command, f.payload);

            Frame ack = {0};
            ack.version = 1;
            ack.sender_id = 0;
            ack.to_hub = 0;
            ack.command = 999;
            snprintf(ack.payload, PAYLOAD_SIZE, "ACK for rover %d", f.sender_id);

            mq_send(rover_out, (char *)&ack, sizeof(Frame), 0);
        }

        usleep(100000);
    }

    return 0;
}
