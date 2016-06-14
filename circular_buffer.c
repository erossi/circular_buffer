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

/*! Clear the buffer.
 */
void cbuffer_clear(struct cbuffer_t *cbuffer)
{
	cbuffer->idx = 0;
	cbuffer->start = 0;
	cbuffer->len = 0;
	cbuffer->flags.b.overflow = FALSE;
}

/*! Initialize the buffer.
 *
 * \param plugin enable the check_eom function plugin.
 * \return the allocated struct.
 */
struct cbuffer_t *cbuffer_init(void)
{
	struct cbuffer_t *cbuffer;

	cbuffer = malloc(sizeof(struct cbuffer_t));
	cbuffer->size = CBUF_SIZE;
	cbuffer->buffer = malloc(CBUF_SIZE);

	if (cbuffer->buffer)
		cbuffer->TOP = CBUF_SIZE - 1;

	cbuffer_clear(cbuffer);
	return(cbuffer);
}

/*! Remove the buffer.
 */
void cbuffer_shut(struct cbuffer_t *cbuffer)
{
	free(cbuffer->buffer);
	free(cbuffer);
}

/*! Copy a byte from the buffer to the message area.
 *
 * data[j] = buffer[start]
 *
 * \note there is not protection, the next char in the buffer will be
 * extracted, event if it should not.
 */
uint8_t bcpy(struct cbuffer_t *cbuffer, uint8_t *data, const uint8_t size, uint8_t j)
{
	if (j < size) {
		*(data + j) = *(cbuffer->buffer + cbuffer->start);
		j++;
	}

#ifdef CBUF_OVR_CHAR
	*(cbuffer->buffer + cbuffer->start) = CBUF_OVR_CHAR;
#endif

	if (cbuffer->start == cbuffer->TOP)
		cbuffer->start = 0;
	else
		cbuffer->start++;

	cbuffer->len--;
	return(j);
}

/*! Get the stored buffer.
 *
 * Fetch from the start_index (cbuffer->start) to the current index (cbuffer->idx).
 *
 * \return the number of bytes received.
 */
uint8_t cbuffer_pop(struct cbuffer_t *cbuffer, uint8_t *data, const uint8_t size)
{
	uint8_t index, j;

	/* freeze the index, while cbuffer->idx can be changed by volatile call to add a
	 * new bytes to the buffer.
	 */
	index = cbuffer->idx;
	j = 0;

	/* Copy the 1st char.
	 * In an overflow condition, the start = index, therefor it
	 * will exit without getting anything back.
	 */
	if (cbuffer->flags.b.overflow)
		j = bcpy(cbuffer, data, size, j);

	while (cbuffer->start != index)
		j = bcpy(cbuffer, data, size, j);

	/* unlock the buffer */
	cbuffer->flags.b.overflow = FALSE;
	return(j);
}

/*! get the message present in the buffer.
 */
uint8_t cbuffer_popm(struct cbuffer_t *cbuffer, uint8_t *data, const uint8_t size,
		const uint8_t eom)
{
	uint8_t index, j, loop;

	/* freeze the index, while cbuffer->idx can be changed by volatile call
	 * to add a new bytes to the buffer.
	 */
	index = cbuffer->idx;
	j = 0;
	loop = TRUE;

	/* Copy the 1st char.
	 * In an overflow condition, the start = index, therefor it
	 * will exit without getting anything back.
	 */
	if (cbuffer->flags.b.overflow) {
		if (*(cbuffer->buffer + cbuffer->start) == eom)
			loop = FALSE;

		j = bcpy(cbuffer, data, size, j);
	}

	while (loop && (cbuffer->start != index)) {
		if (*(cbuffer->buffer + cbuffer->start) == eom)
			loop = FALSE;

		j = bcpy(cbuffer, data, size, j);
	}

	/* unlock the buffer */
	cbuffer->flags.b.overflow = FALSE;
	return(j);
}

/*! add data to the buffer.
 *
 * \note if overflow and EOM then the last char must be the EOM and the
 * cbuffer->flags.b.msgs must be set.
 */
uint8_t cbuffer_push(struct cbuffer_t *cbuffer, char rxc)
{
	/* If the buffer is full (overflow flag)
	 * do nothing.
	 */
	if (cbuffer->flags.b.overflow) {
		return(FALSE);
	} else {
		/* catch overflow */
		if (cbuffer->start) {
			/* idx next to start? */
			if (cbuffer->idx == (cbuffer->start - 1))
				cbuffer->flags.b.overflow = TRUE;
		} else {
			/* idx next to the end of the buffer? */
			if (cbuffer->idx == cbuffer->TOP)
				cbuffer->flags.b.overflow = TRUE;
		}

		*(cbuffer->buffer + cbuffer->idx) = rxc;

		if (cbuffer->idx == cbuffer->TOP)
			cbuffer->idx = 0;
		else
			cbuffer->idx++;

		cbuffer->len++;
		return(TRUE);
	}
}
