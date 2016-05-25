/*
    Circular Buffer, a string oriented circular buffer implementation.
    Copyright (C) 2015, 2016 Enrico Rossi

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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "circular_buffer.h"

void cbuffer_clear(struct cbuffer_t *cbuffer)
{
	cbuffer->idx = 0;
	cbuffer->start = 0;
	cbuffer->msgs = 0;
	cbuffer->overflow = FALSE;
}

struct cbuffer_t *cbuffer_init(void)
{
	struct cbuffer_t *cbuffer;

	cbuffer = malloc(sizeof(struct cbuffer_t));
	cbuffer->buffer = malloc(CBUF_SIZE);

	if (cbuffer->buffer)
		cbuffer->TOP = CBUF_SIZE - 1;

	cbuffer_clear(cbuffer);
	return(cbuffer);
}

void cbuffer_shut(struct cbuffer_t *cbuffer)
{
	free(cbuffer->buffer);
	free(cbuffer);
}

uint8_t cbuffer_getdata(struct cbuffer_t *cbuffer, uint8_t *data, const uint8_t size)
{
	uint8_t index, j;

	/* freeze the index, while cbuffer->idx can be changed by volatile call to add a
	 * new bytes to the buffer.
	 */
	index = cbuffer->idx;
	j = 0;

	/* if there are data in the buffer */
	if (cbuffer->start != index) {
		/*!
		 * if a wrap is needed then
		 *  - copy from start to the TOP of the buffer plus
		 *  - copy from the beginning of the buffer to the index.
		 * else
		 *  - copy from the start to the current index
		 */
		if (cbuffer->start > index) {
			while (cbuffer->start < cbuffer->TOP) {
				if (j < size) {
					*(data + j) = *(cbuffer->buffer + cbuffer->start);
					j++;
				}

				cbuffer->start++;
			}

			cbuffer->start = 0;

			while (cbuffer->start < index) {
				if (j < size) {
					*(data + j) = *(cbuffer->buffer + cbuffer->start);
					j++;
				}

				cbuffer->start++;
			}
		} else {
			while (cbuffer->start < index) {
				if (j < size) {
					*(data + j) = *(cbuffer->buffer + cbuffer->start);
					j++;
				}

				cbuffer->start++;
			}
		}

		/* unlock the buffer */
		cbuffer->overflow = FALSE;
	}

	return(j);
}

#ifdef CBUF_EOM
/*! get the message present in the buffer.
 *
 * Assumptions: if there is a message in the buffer then an EOM char will be found.
 *
 */
uint8_t cbuffer_getmsg(struct cbuffer_t *cbuffer, char *message, const uint8_t size)
{
	uint8_t j, eom;

	eom = FALSE;

	/* if a msg is present */
	if (cbuffer->msgs) {
		j = 0;

		/* Copy from start to the end of the buffer, or
		 * until an EOM is found.
		 */
		while (!eom && (cbuffer->start < CBUF_SIZE)) {
			/* copy only if there is space left */
			if (j < size) {
				*(message + j) = *(cbuffer->buffer + cbuffer->start);
				j++;
			}

			/* if end of msg */
			if (*(cbuffer->buffer + cbuffer->start) == CBUF_EOM)
				eom=TRUE;

#ifdef CBUF_OVR_CHAR
			*(cbuffer->buffer + cbuffer->start) = CBUF_OVR_CHAR;
#endif /* CBUF_OVR_CHAR */

			if (cbuffer->start == cbuffer->TOP)
				cbuffer->start = 0;
			else
				cbuffer->start++;
		}

		/* wrap condition, if we haven't found EOM yet */
		while (!eom && (cbuffer->start < CBUF_SIZE)) {
			/* copy only if there is space left */
			if (j < size) {
				*(message + j) = *(cbuffer->buffer + cbuffer->start);
				j++;
			}

			/* if end of msg */
			if (*(cbuffer->buffer + cbuffer->start) == CBUF_EOM)
				eom=TRUE;

#ifdef CBUF_OVR_CHAR
			*(cbuffer->buffer + cbuffer->start) = CBUF_OVR_CHAR;
#endif /* CBUF_OVR_CHAR */

			if (cbuffer->start == cbuffer->TOP)
				cbuffer->start = 0;
			else
				cbuffer->start++;
		}

		cbuffer->msgs--;
		/* unlock the buffer */
		cbuffer->overflow = FALSE;
	}

#ifdef CBUF_SAFE_EOM
	/* Put an EOM at the end of the destination buffer for security */
	*(message + size - 1) = 0;
#endif /* CBUF_SAFE_EOM */

	return(eom);
}

#endif /* CBUF_EOM */

/* add data to the buffer.
 */
uint8_t cbuffer_add(struct cbuffer_t *cbuffer, char rxc)
{
	/* If the buffer is full (overflow flag)
	 * do nothing.
	 */
	if (cbuffer->overflow) {
		return(FALSE);
	} else {
		/* catch overflow */
		if (cbuffer->start) {
			/* idx next to start? */
			if (cbuffer->idx == (cbuffer->start - 1))
				cbuffer->overflow = TRUE;
		} else {
			/* idx next to the end of the buffer? */
			if (cbuffer->idx == cbuffer->TOP)
				cbuffer->overflow = TRUE;
		}

#ifdef CBUF_EOM
		/* if overflow discard the current byte and then set the EOM */
		if (cbuffer->overflow)
			rxc = CBUF_EOM;

		/* if EOM then increase the message counter */
		if (rxc == CBUF_EOM)
			cbuffer->msgs++;
#endif

		*(cbuffer->buffer + cbuffer->idx) = rxc;

		if (cbuffer->idx == cbuffer->TOP)
			cbuffer->idx = 0;
		else
			cbuffer->idx++;

		return(TRUE);
	}
}
