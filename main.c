#include <avr/common.h>
#include <avr/io.h>
#include <util/delay.h>
#include "sensor_node.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define TX_PORT PORTB
#define TX_TOGGLE_REG PINB
#define TX_PIN PB3

#define TEMP_SENSE_POWER_PORT PORTB
#define TEMP_SENSE_POWER_PIN PB0

#define TEMP_SENSE_INPUT_DIG_PIN PB4
#define TEMP_SENSE_INPUT_AN_PIN 2

void transmit(void);

#define HALF_BIT_DELAY_TIME_MILLIS 0.5

static volatile char adc_done;

ISR(ADC_vect)
{
    adc_done = 1;
}

inline static void do_adc_conversion() {
    adc_done = 0;
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_mode();
    while (!adc_done) {
        // Just in case we were woken by a different interrupt.
    }
}

inline static uint16_t read_temperature() {
    // Power up the temperature sensor.
    TEMP_SENSE_POWER_PORT |= _BV(TEMP_SENSE_POWER_PIN);

    // Configure the ADC.
    ADMUX = _BV(REFS0) | /* 1.1V reference */
            TEMP_SENSE_INPUT_AN_PIN /* Pin selection */;
    ADCSRA = _BV(ADEN) | /* Enable ADC */
             _BV(ADIE) | /* Enable completion interrupt. */
             6           /* Select CLK/32 prescaler. */;

    // Trigger a conversion but discard the result because the first conversion
    // can be garbage.
    do_adc_conversion();

    // Use the second conversion.  Kick it off by entering sleep mode.
    do_adc_conversion();
    uint16_t result = ADCW;

    // Disable ADC.
    ADCSRA &= ~_BV(ADEN);

    // Power down the temp sensor.
    TEMP_SENSE_POWER_PORT &= ~_BV(TEMP_SENSE_POWER_PIN);

    return result;
}

int main(void)
{
    // Configure all pins as outputs except the temperature input pin.
    DDRB = 0xFF ^ _BV(TEMP_SENSE_INPUT_DIG_PIN);
    // Disable digital input buffer on the analog input pin.
    DIDR0 |= _BV(TEMP_SENSE_INPUT_DIG_PIN);

    // Enable interrupts.
    sei();

    while (1)
    {
        manchester_union.manchester_packet.node_id = 1;
        manchester_union.manchester_packet.seq_no += 1;
        manchester_union.manchester_packet.reading_type = READING_TYPE_TEMP;
        manchester_union.manchester_packet.reading = read_temperature();
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

