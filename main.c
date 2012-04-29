#include <avr/common.h>
#include <avr/io.h>
#include <util/delay.h>
#include "sensor_node.h"

#define TX_PORT PORTB
#define TX_PIN PB3

void transmit(void);

#define HALF_BIT_DELAY_TIME_MILLIS 1.0

struct manchester_packet {
    manchester_data_t data;
    uint16_t checksum;
} __attribute__((packed));

static union {
    struct manchester_packet manchester_packet;
    char manchester_data[sizeof(struct manchester_packet)];
} manchester_union;

int main(void)
{
    DDRB = 255;
    while (1)
    {
        manchester_union.manchester_packet.data.node_id = 1;
        manchester_union.manchester_packet.data.seq_no += 1;
        manchester_union.manchester_packet.data.reading_type = READING_TYPE_TEMP;
        manchester_union.manchester_packet.data.reading = 0xBEEF;
        transmit();
        _delay_ms(500);
    }

    return (1); // should never happen
}

// Transmit one byte using manchester encoding.  Sends LSB first.
void transmit_byte(uint8_t b)
{
    for (char j = 0; j < 8; j++)
    {
        if (b & 1)
        {
            // Transmit a 1 bit.  High then low.
            TX_PORT |= (char)_BV(TX_PIN);
        }
        else
        {
            // Transmit a 0 bit.  Low then high.
            TX_PORT &= (char)~_BV(TX_PIN);
        }
        _delay_ms(HALF_BIT_DELAY_TIME_MILLIS);
        // Transmit the other half of the bit.
        TX_PORT ^= (char)_BV(TX_PIN);
        _delay_ms(HALF_BIT_DELAY_TIME_MILLIS);

        b >>= 1;
    }
}

void transmit(void)
{
    // Transmit preamble
    for (int i = 10; i > 0; i--) {
        transmit_byte(0xFF);
    }
    transmit_byte(0x7F);

    // Transmit data
    int len = sizeof(struct manchester_packet);
    for (int i = 0; i < len; i++)
    {
        char b = manchester_union.manchester_data[i];
        transmit_byte(b);
    }
}

