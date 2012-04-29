#include <avr/common.h>
#include <avr/io.h>
#include <util/delay.h>
#include "sensor_node.h"

#define TX_PORT PORTB
#define TX_PIN PB3

void transmit(void);

#define HALF_BIT_DELAY_TIME_MILLIS 0.5

int main(void)
{
    DDRB = 255;
    while (1)
    {
        manchester_union.manchester_packet.node_id = 1;
        manchester_union.manchester_packet.seq_no += 1;
        manchester_union.manchester_packet.reading_type = READING_TYPE_TEMP;
        manchester_union.manchester_packet.reading = 0xBEEF;
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

inline void transmit(void)
{
    manchester_union.manchester_packet.checksum = calculate_checksum(&manchester_union.manchester_packet);

    // Transmit preamble
    for (int i = 4; i > 0; i--) {
        transmit_byte(0xFF);
    }
    transmit_byte(0x7F);

    // Transmit data
    int len = sizeof(manchester_union);
    for (int i = 0; i < len; i++)
    {
        char b = manchester_union.manchester_data[i];
        transmit_byte(b);
    }
    // Generate one final transition
    TX_PORT ^= (char)_BV(TX_PIN);
    _delay_ms(HALF_BIT_DELAY_TIME_MILLIS);
    // Then leave the line high.
    TX_PORT |= (char)_BV(TX_PIN);
}

