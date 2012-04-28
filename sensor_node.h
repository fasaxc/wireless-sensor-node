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
} __attribute__((packed)) manchester_data_t;

#endif /* SENSOR_NODE_H_ */
