/*
   Circular Buffer, a string oriented circular buffer implementation.
   Copyright (C) 2017 Enrico Rossi

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
void CBuffer::clear()
{
	idx_ = 0;
	start_ = 0;
	len_ = 0;
	overflow_ = false;
}

/*! Initialize the buffer.
 *
 * \param plugin enable the check_eom function plugin.
 * \return the allocated struct.
 */
CBuffer::CBuffer(uint8_t sz) : size_{sz}
{
	buffer_ = std::make_unique<uint8_t[]>(size_);
	TOP_ = size_ - 1;
	clear();
}

/*! Copy a byte from the buffer to the message area.
 *
 * data[j] = buffer[start]
 *
 * \param data the area where to copy the char.
 * \param datasize sizeof(data)
 * \param j the index used as data[j].
 * \return the updated index for data.
 *
 * \warning CBuffer::start gets modified.
 * \note there is not protection, the next char in the buffer will be
 * extracted and indexes modified event if it should not.
 */
uint8_t CBuffer::bcpy(uint8_t *data, const uint8_t datasize, uint8_t j)
{
	// is there enough space in data?
	if (j < datasize) {
		*(data + j) = buffer_[start_];
		j++;
	}

#ifdef CBUF_OVR_CHAR
	buffer_[start_] = CBUF_OVR_CHAR;
#endif

	if (start_ == TOP_)
		start_ = 0;
	else
		start_++;

	len_--;
	return (j);
}

/*! Get the stored buffer.
 *
 * Fetch from the start_index (cbuffer->start) to the current index (cbuffer->idx).
 *
 * \param data the area where to copy the message if found.
 * \param datasize sizeof(data)
 * \return the number of bytes fetched.
 */
uint8_t CBuffer::pop(uint8_t* data, const uint8_t datasize)
{
	uint8_t index, j;

	j = 0;

	// if there are chars to fetch
	if (len_) {
		/* freeze the index, while cbuffer->idx can be changed by
		 * volatile call to add a new bytes to the buffer.
		 */
		index = idx_;

		/* Copy the 1st char.
		 * In an overflow condition, the start = index, therefor it
		 * will exit without getting anything back.
		 */
		if (overflow_)
			j = bcpy(data, datasize, j);

		while ((start_ != index) && (j < datasize))
			j = bcpy(data, datasize, j);

		/* unlock the buffer */
		overflow_ = false;
	}

	return (j);
}

/*! get a message present in the buffer.
 *
 * If no EOM is found then all the content of the buffer
 * is copied and no EOM or \0 is added to the end.
 *
 * If the size of *data is less then the message in the buffer, then
 * *data get filled and the rest of the message is lost.
 *
 * \param cbuffer the circular buffer.
 * \param data the area where to copy the message if found.
 * \param size sizeof(data)
 * \param eom the EndOfMessage char.
 * \return the number of char copied.
 *
 * \warning if the *data is filled, no EOM or \0 is added at the end.
 */
uint8_t CBuffer::popm(uint8_t* data, const uint8_t datasize, const uint8_t eom)
{
	bool loop {false};
	uint8_t index;
	uint8_t j {0};

	/* if there is something in the buffer */
	if (len_) {
		/* freeze the index, while cbuffer->idx can be changed by
		 * volatile call to add a new bytes to the buffer.
		 */
		index = idx_;
		loop = true;

		/* Check if overflow and copy the 1st char manually.
		 * In an overflow condition, start == index, therefore
		 * no chars will be copied normally.
		 */
		if (overflow_) {
			/* if the 1st char is an EOM, copy it and exit */
			if (buffer_[start_] == eom)
				loop = false;

			/* copy a byte, CBuffer::start changed */
			j = bcpy(data, datasize, j);
		}

		/* Extract all the date in the buffer until:
		 * - EOM is found or
		 * - buffer is empty.
		 *
		 * WARN: do NOT merge this with the previous in a
		 * do..while loop, because bcpy() modifies cbuffer
		 * indexes.
		 */
		while (loop && (start_ != index)) {
			/* if an EOM is found.
			 * \note need to be done before bcpy() because
			 * cbuffer->start gets modified later.
			 */
			if (buffer_[start_] == eom)
				loop = false;

			/* copy a byte, cbuffer->start changed */
			j = bcpy(data, datasize, j);
		}

		/* unlock the buffer */
		overflow_ = false;
	}

	return (j);
}

/*! add data to the buffer.
 *
 * \note if overflow and EOM then the last char must be the EOM.
 */
bool CBuffer::push(char rxc)
{
	/* If the buffer is full (overflow flag)
	 * do nothing.
	 */
	if (overflow_) {
		return (false);
	} else {
		/* catch overflow */
		if (start_) {
			/* idx next to start? */
			if (idx_ == (start_ - 1))
				overflow_ = true;
		} else {
			/* idx next to the end of the buffer? */
			if (idx_ == TOP_)
				overflow_ = true;
		}

		buffer_[idx_] = (uint8_t)rxc;

		if (idx_ == TOP_)
			idx_ = 0;
		else
			idx_++;

		len_++;
		return (true);
	}
}
