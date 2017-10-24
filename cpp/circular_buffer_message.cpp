/*
   Circular Buffer, a byte oriented circular buffer implementation.
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
#include "circular_buffer_message.h"

/*! Pop everything from start_ to EOM.
 *
 * If no EOM is found then all the content of the buffer
 * is copied like pop().
 *
 * If the size of data is less then the message in the buffer, then
 * data get filled and the rest of the message is left in the buffer.
 *
 * \param data the area where to copy the message.
 * \param sizeofdata.
 * \param eom the EndOfMessage.
 * \return the number of byte copied.
 *
 * \note EOM is NOT copied.
 * \warning race condition!
 */
uint8_t CBufferM::popm(uint8_t* data, const uint8_t sizeofdata, const uint8_t eom)
{
	uint8_t j {0};

	// while there is space left on data
	//  and byte in the buffer
	//  and the data fetched is NOT EOM
	while ((j < sizeofdata) && popc(data + j) && (*(data + j) != eom))
		j++;

	return (j);
}
