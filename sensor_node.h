/*
 * Copyright (c) 2012, Shaun Crampton
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
 
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
