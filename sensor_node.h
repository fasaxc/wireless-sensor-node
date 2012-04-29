#ifndef SENSOR_NODE_H_
#define SENSOR_NODE_H_

#define READING_TYPE_TEMP 't'
#define READING_TYPE_LIGHT 'l'
#define READING_TYPE_HUMIDITY 'h'
#define READING_TYPE_BATTERY 'b'

typedef struct
{
    uint8_t node_id;
    uint8_t seq_no;
    char reading_type;
    uint16_t reading;
    uint8_t checksum;
} __attribute__((packed)) manchester_data_t;

union manchester_union {
    manchester_data_t manchester_packet;
    char manchester_data[sizeof(manchester_data_t)];
} manchester_union;

const uint8_t CRC_POLY = 0x07;

inline uint8_t calculate_checksum(manchester_data_t *packet) {
    uint8_t *p = (uint8_t *)packet;
    uint8_t check = 0;
    while (p < &packet->checksum) {
        uint8_t b = *p;
        for (uint8_t i = 8; i > 0; i--) {
            if (check & 0x80) {
                check <<= 1;
                check |= b & 1;
                check ^= CRC_POLY;
            } else {
                check <<= 1;
                check |= b & 1;
            }
            b >>= 1;
        }
        p++;
    }
    return check;
}

#endif /* SENSOR_NODE_H_ */
