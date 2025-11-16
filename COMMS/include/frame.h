#ifndef FRAME_H
#define FRAME_H

#define PAYLOAD_SIZE 64

typedef struct {
    int version;
    int sender_id;
    int to_hub; // 1 = message to HUB, 0 = message to ROVERS
    int command;
    char payload[PAYLOAD_SIZE];
} Frame;

#endif