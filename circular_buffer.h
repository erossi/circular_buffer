/*
    Circular Buffer, a string oriented circular buffer implementation.
    Copyright (C) 2015 Enrico Rossi

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdint.h>

#ifndef CBUFFER_H
#define CBUFFER_H

/*! The size of the buffer */
#ifndef CBUF_SIZE
#define CBUF_SIZE 20
#endif

/*!
 * The char that replace every char read.
 *
 * Usefull to show which part of the circular buffer has been
 * read.
 */
#define CBUF_OVR_CHAR 'x'

/* to force message_get to terminate an unterminated message
 * due to end of buffer reached, then
 * #define CBUF_SAFE_EOM
 * or add "-D CBUF_SAFE_EOM" at compile time.
 */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

struct cbuffer_t {
	char *buffer;
	uint8_t idx;
	uint8_t start;
	uint8_t msgs;
	uint8_t FLowf;
};

void cbuffer_clear(struct cbuffer_t *cbuffer);
struct cbuffer_t *cbuffer_init(void);
void cbuffer_shut(struct cbuffer_t *cbuffer);
uint8_t cbuffer_getmsg(struct cbuffer_t *cbuffer, char *message,
		const uint8_t size);
uint8_t cbuffer_add(struct cbuffer_t *cbuffer, char rxc);

#endif
