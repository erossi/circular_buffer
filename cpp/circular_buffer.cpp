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

#include <stdint.h>
#include "circular_buffer.h"

//! Clear the buffer.
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

/*! Extract a single byte from the buffer.
 *
 * data = buffer[start]
 *
 * \param data the area where to copy the byte.
 * \return true if ok
 * \warning possible race condition!
 */
bool CBuffer::popc(uint8_t *data)
{
	if (len_) {
		*data = buffer_[start_];

#ifdef CBUF_OVR_CHAR
		buffer_[start_] = CBUF_OVR_CHAR;
#endif

		if (start_ == TOP_)
			start_ = 0;
		else
			start_++;

		// From here possible race condition with push().
		len_--;
		overflow_ = false;

		return (true);
	} else {
		return (false);
	}
}

/*! Pop everything present in the buffer.
 *
 * start_ to the current idx_.
 *
 * \param data the area where to copy the message if found.
 * \param sizeofdata.
 * \return the number of bytes fetched.
 * \warning if the data size is less than the buffer, only the sizeofdata
 * byte get fetched, the buffer remain not empty.
 */
uint8_t CBuffer::pop(uint8_t* data, const uint8_t sizeofdata)
{
	uint8_t j {0};

	// while there is space left on data and byte in the buffer
	while ((j < sizeofdata) && popc(data + j))
		j++;

	return (j);
}

/*! add data to the buffer.
 *
 * \note if overflow and EOM then the last char must be the EOM.
 *
 * \warning race condition with other functions.
 *  keep an eye on: overflow_, idx_, buffer_[idx_], len_
 */
bool CBuffer::push(uint8_t c)
{
	// If the buffer is full do nothing.
	if (overflow_) {
		return (false);
	} else {
		// catch overflow
		if (start_) {
			// idx next to start?
			if (idx_ == (start_ - 1))
				overflow_ = true;
		} else {
			// idx next to the end of the buffer?
			if (idx_ == TOP_)
				overflow_ = true;
		}

		buffer_[idx_] = c;

		if (idx_ == TOP_)
			idx_ = 0;
		else
			idx_++;

		len_++;
		return (true);
	}
}
