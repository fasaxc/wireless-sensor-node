#include <avr/common.h>
#include <avr/io.h>
#include <util/delay_basic.h>

#define TX_PORT PORTB
#define TX_PIN PB3

inline static void init_transmit(void);
void transmit(void);

int main(void)
{
    DDRB = 255;
    init_transmit();
    while (1)
    {
        PORTB = 255;
        _delay_loop_2(0);
        PORTB = 0;
        _delay_loop_2(0);
        transmit();
    }

    return (1); // should never happen
}

// Maximum rate at which the transmitter can transmit (bits/s)
#define TX_BAUD (4800)
#define CLOCK_TICKS_PER_TX_BIT (F_CPU / TX_BAUD)
#define CLOCK_TICKS_PER_DELAY_LOOP (3)
#define DELAY_PER_TX_BIT (CLOCK_TICKS_PER_TX_BIT / CLOCK_TICKS_PER_DELAY_LOOP)

#define PACKET_LENGTH 5

struct manchester_packet {
    char preamble1;
    char preamble2;
    char data[PACKET_LENGTH];
} __attribute__((packed));

static union {
    struct manchester_packet manchester_packet;
    char manchester_data[sizeof(struct manchester_packet)];
} manchester_union;

inline static void init_transmit(void)
{
    manchester_union.manchester_packet.preamble1 = 0xAA;
    manchester_union.manchester_packet.preamble2 = 0xA9;
}

void transmit(void)
{
    int len = sizeof(struct manchester_packet);
    for (int i = 0; i < len; i++)
    {
        char mask = manchester_union.manchester_data[i];
        for (char j = 0; j < 8; j++)
        {
            if (mask & 1)
            {
                // Transmit a 1 bit.  High then low.
                TX_PORT |= _BV(TX_PIN);
            }
            else
            {
                // Transmit a 0 bit.  Low then high.
                TX_PORT &= ~_BV(TX_PIN);
            }
            _delay_loop_1(DELAY_PER_TX_BIT);
            TX_PORT ^= _BV(TX_PIN);
            _delay_loop_1(DELAY_PER_TX_BIT);

            mask >>= 1;
        }
    }
}
