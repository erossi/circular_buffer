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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "circular_buffer.h"

struct cbuffer_t *cbuffer_init(void)
{
	struct cbuffer_t *cbuffer;

	cbuffer = malloc(sizeof(struct cbuffer_t));
	cbuffer->buffer = malloc(CBUF_SIZE);
	cbuffer->idx=0;
	cbuffer->msgs=0;
	cbuffer->start=0;
	cbuffer->FLowf=FALSE;
	return(cbuffer);
}

void cbuffer_shut(struct cbuffer_t *cbuffer)
{
	free(cbuffer->buffer);
	free(cbuffer);
}

uint8_t cbuffer_getmsg(struct cbuffer_t *cbuffer, char *message)
{
	uint8_t i, j;
	uint8_t FLeom;

	FLeom = FALSE;

	/* if a msg is present */
	if (cbuffer->msgs) {
		j = 0;
		i = cbuffer->start;

		while (!FLeom && (i < CBUF_SIZE)) {
			*(message + j) = *(cbuffer->buffer + i);

			/* if end of msg */
			if (! *(cbuffer->buffer + i)) {
				FLeom=TRUE;

				if (i == CBUF_SIZE-1)
					cbuffer->start = 0;
				else
					cbuffer->start = i + 1;
			}

#ifdef CBUF_OVR_CHAR
			*(cbuffer->buffer + i) = CBUF_OVR_CHAR;
#endif

			i++;
			j++;
		}

		i = 0;

		while (!FLeom && (i < cbuffer->start)) {
			*(message + j) = *(cbuffer->buffer + i);

			if (! *(cbuffer->buffer + i)) {
				FLeom=TRUE;
				cbuffer->start = i + 1;
			}

#ifdef CBUF_OVR_CHAR
			*(cbuffer->buffer + i) = CBUF_OVR_CHAR;
#endif

			i++;
			j++;
		}

		cbuffer->msgs--;
		cbuffer->FLowf = FALSE;
	}

	return(FLeom);
}

uint8_t cbuffer_add(struct cbuffer_t *cbuffer, char rxc)
{
	if (cbuffer->FLowf) {
		return(FALSE);
	} else {
		/* if the start idx > 0 */
		if (cbuffer->start) {
			/* idx next to start? */
			if (cbuffer->idx == cbuffer->start-1) {
				cbuffer->FLowf = TRUE;
				rxc = 0;
			}
		} else {
			/* idx next to the end of the buffer? */
			if (cbuffer->idx == CBUF_SIZE-1) {
				cbuffer->FLowf = TRUE;
				rxc = 0;
			}
		}

		if (!rxc)
			cbuffer->msgs++;

		*(cbuffer->buffer + cbuffer->idx) = rxc;

		if (cbuffer->idx == CBUF_SIZE-1)
			cbuffer->idx = 0;
		else
			cbuffer->idx++;

		return(TRUE);
	}
}
