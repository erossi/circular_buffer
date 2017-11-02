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

#ifndef CBUFFER_SHADOW_H
#define CBUFFER_SHADOW_H

#include "circular_buffer.h"

/*! Buffer structure
 
 [ | | | | | | | | | | | | | | | | | | | | | | | | ]
  ^buffer   ^start    ^shadow_start       ^idx    ^TOP
                      ^ --- shadow_len ---^
            ^------------ len ------------^
  ^---------------------- size -------------------^
 */

template <typename T, typename D>
class CBufferS : public CBuffer<T, D> {
	private:
		T shadow_start_;
		T shadow_len_;
	public:
		// Debugging methods
		T start() const { return(shadow_start_); };
		T len() const { return(shadow_len_); };

		// Contructor
		// force the Base constructor with the parameter size
		// or it will call the default with no parameters and
		// it will throw and error if initialiazed with one.
		CBufferS(T size = CBUF_SIZE);

		// declaration overload for shadow index.
		void clear() override;
		bool popc(D*);
		T pop(D*, const T);
		bool push(D);

		// new member functions
		void commit();
		void reset();
};

//! Clear the buffer and the shadow index.
//
// \sameas CBuffer::clear()
//
template <typename T, typename D>
void CBufferS<T, D>::clear()
{
	CBuffer<T, D>::clear(); // call the base clear
	shadow_start_ = 0;
	shadow_len_ = 0;
}

//! Contruct the buffer with the shadow index.
template <typename T, typename D>
CBufferS<T, D>::CBufferS(T size) : CBuffer<T, D>{size}
{
	shadow_start_ = 0;
	shadow_len_ = 0;
}

/*! Extract a single shadow object from the buffer.
 *
 * \warning possible race condition!
 */
template <typename T, typename D>
bool CBufferS<T, D>::popc(D *data)
{
	if (shadow_len_) {
		// Here "this->" could be used since operator[] has not
		// been overloaded.
		*data = CBuffer<T, D>::operator[](shadow_start_);

		// Here "this->" could be used since TOP_ has not
		// been overloaded.
		if (shadow_start_ == CBuffer<T, D>::TOP_)
			shadow_start_ = 0;
		else
			shadow_start_++;

		// From here possible race condition with push().
		shadow_len_--;

		return (true);
	} else {
		return (false);
	}
}

/*! Pop a copy of everything present in the buffer.
 *
 * \warning if the data size is less than the buffer, only the sizeofdata
 * byte get fetched, the buffer remain not empty.
 */
template <typename T, typename D>
T CBufferS<T, D>::pop(D* data, const T sizeofdata)
{
	T j {0};

	// while there is space left on data and byte in the buffer
	while ((j < sizeofdata) && popc(data + j))
		j++;

	return (j);
}

/*! add data to the buffer and update the shadow indexes.
 *
 * \warning race condition with other functions.
 *  modified CBufferS members data: shadow_len_
 */
template <typename T, typename D>
bool CBufferS<T, D>::push(D c)
{
	if (CBuffer<T, D>::push(c)) {
		shadow_len_++;
		return(true);
	} else {
		return(false);
	}
}

/*! Commit the shadow index operations.
 *
 * Uses the protected CBuffer member functions.
 */
template <typename T, typename D>
void CBufferS<T, D>::commit()
{
	CBuffer<T, D>::set_start(shadow_start_);
	CBuffer<T, D>::set_len(shadow_len_);
}

/*! Restore the index back.
 *
 */
template <typename T, typename D>
void CBufferS<T, D>::reset()
{
	shadow_start_ = CBuffer<T, D>::start();
	shadow_len_ = CBuffer<T, D>::len();
}

#endif
