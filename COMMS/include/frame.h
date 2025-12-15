#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#define PAYLOAD_SIZE 32

typedef struct {
    uint8_t version; // protocol version
    uint8_t msg_type; // COMMAND, STATUS, HEARTBEAT
    uint8_t sender_id; // rover_id or hub_id
    uint8_t target_id; // 0 = hub, other = rover_id
    uint32_t timestamp; // simple unix time or counter
    uint8_t payload_len; // bytes of payload
    char payload[PAYLOAD_SIZE]; // actual payload
    uint8_t crc; // checksum
} Frame;

#endif