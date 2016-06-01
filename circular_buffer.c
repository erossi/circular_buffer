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

/*! Preprocess the rxc with EOM.
 *
 * If overflow is set then the rx char is set to EOM.
 * If the rx char is an EOM increment the message counter (message oriented rx).
 */
uint8_t check_eom(struct cbuffer_t *cbuffer, char rxc)
{
	/* if overflow discard the current byte and then set the EOM */
	if (cbuffer->flags.value.overflow)
		rxc = CBUF_EOM;

	/* if EOM then increase the message counter */
	if (rxc == CBUF_EOM)
		cbuffer->flags.value.msgs++;

	return(rxc);
}

/*! Clear the buffer.
 */
void cbuffer_clear(struct cbuffer_t *cbuffer)
{
	cbuffer->idx = 0;
	cbuffer->start = 0;
	cbuffer->flags.value.msgs = 0;
	cbuffer->flags.value.overflow = FALSE;
}

/*! Initialize the buffer.
 *
 * \param plugin enable the check_eom function plugin.
 * \return the allocated struct.
 */
struct cbuffer_t *cbuffer_init(uint8_t plugin)
{
	struct cbuffer_t *cbuffer;

	cbuffer = malloc(sizeof(struct cbuffer_t));
	cbuffer->buffer = malloc(CBUF_SIZE);

	if (cbuffer->buffer)
		cbuffer->TOP = CBUF_SIZE - 1;

	cbuffer_clear(cbuffer);

	if (plugin)
		cbuffer->preprocess_rx = check_eom;
	else
		cbuffer->preprocess_rx = NULL;

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

	return(j);
}

/*! Get binary data stored in the buffer.
 *
 * Fetch from the start_index (cbuffer->start) to the current index (cbuffer->idx).
 *
 * \warning if used with EOM plugin, cbuffer->flags.value.msgs must be manually cleared.
 */
uint8_t cbuffer_pop(struct cbuffer_t *cbuffer, uint8_t *data, const uint8_t size)
{
	uint8_t index, j;

	/* freeze the index, while cbuffer->idx can be changed by volatile call to add a
	 * new bytes to the buffer.
	 */
	index = cbuffer->idx;
	j = 0;

	if (cbuffer->flags.value.overflow)
		j = bcpy(cbuffer, data, size, j);

	while (cbuffer->start != index)
		j = bcpy(cbuffer, data, size, j);

	/* unlock the buffer */
	cbuffer->flags.value.overflow = FALSE;
	return(j);
}

/*! get the message present in the buffer.
 *
 * Assumptions: if there is a message in the buffer then an EOM char will be found.
 *
 * \warning if an EOM is not present in the buffer, this function will infinite-loop.
 */
uint8_t cbuffer_popm(struct cbuffer_t *cbuffer, char *message, const uint8_t size)
{
	uint8_t j, eom;

	eom = FALSE;

	/* if a msg is present */
	if (cbuffer->flags.value.msgs) {
		j = 0;

		/* Copy from start to the end of the buffer, or
		 * until an EOM is found.
		 */
		while (!eom) {
			/* check if EOM */
			if (*(cbuffer->buffer + cbuffer->start) == CBUF_EOM)
				eom=TRUE;

			j = bcpy(cbuffer, (uint8_t *)message, size, j);
		}

		cbuffer->flags.value.msgs--;
		/* unlock the buffer */
		cbuffer->flags.value.overflow = FALSE;
	}

	/* Put an EOM at the end of the destination buffer for security */
	*(message + size - 1) = 0;

	return(eom);
}

/*! add data to the buffer.
 *
 * \note if overflow and EOM then the last char must be the EOM and the
 * cbuffer->flags.value.msgs must be set.
 */
uint8_t cbuffer_push(struct cbuffer_t *cbuffer, char rxc)
{
	/* If the buffer is full (overflow flag)
	 * do nothing.
	 */
	if (cbuffer->flags.value.overflow) {
		return(FALSE);
	} else {
		/* catch overflow */
		if (cbuffer->start) {
			/* idx next to start? */
			if (cbuffer->idx == (cbuffer->start - 1))
				cbuffer->flags.value.overflow = TRUE;
		} else {
			/* idx next to the end of the buffer? */
			if (cbuffer->idx == cbuffer->TOP)
				cbuffer->flags.value.overflow = TRUE;
		}

		/* add the preprocess plugin */
		if (cbuffer->preprocess_rx)
			rxc = cbuffer->preprocess_rx(cbuffer, rxc);

		*(cbuffer->buffer + cbuffer->idx) = rxc;

		if (cbuffer->idx == cbuffer->TOP)
			cbuffer->idx = 0;
		else
			cbuffer->idx++;

		return(TRUE);
	}
}
