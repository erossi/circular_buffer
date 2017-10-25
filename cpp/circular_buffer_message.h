/*
 * Circular Buffer, an object oriented circular buffer.
 * Copyright (C) 2015-2017 Enrico Rossi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CBUFFER_MSG_H
#define CBUFFER_MSG_H

#include "circular_buffer.h"

template <typename T, typename D>
class CBufferM : public CBuffer<T, D> {
	public:
		// Contructor
		// force the Base constructor with the parameter size
		// or it will call the default with no parameters and
		// it will throw and error if initialiazed with one.
		CBufferM(T size = CBUF_SIZE) : CBuffer<T, D>{size} {};
		T popm(D*, const T, const D);
};

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
template <typename T, typename D>
T CBufferM<T, D>::popm(D* data, const T sizeofdata, const D eom)
{
	T j {0};

	// while there is space left on data
	//  and byte in the buffer
	//  and the data fetched is NOT EOM
	while ((j < sizeofdata) && this->popc(data + j) && (*(data + j) != eom))
		j++;

	return (j);
}

#endif
