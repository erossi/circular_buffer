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

#ifndef CBUFFER_H
#define CBUFFER_H

#include <memory>

#ifndef CBUF_SIZE // Default buffer size
#define CBUF_SIZE 16
#endif

/*! Buffer structure

 [ | | | | | | | | | | | | | | | | | | | | | | | | ]
  ^buffer   ^start                        ^idx    ^TOP
            ^------------ len ------------^
  ^---------------------- size -------------------^
 */

// CBuffer of D objects indexed by T type.
template <typename T, typename D>
class CBuffer {
	private:
		// Fixed array size
		std::unique_ptr<D[]> buffer_;
		T idx_;
		T start_;
		bool overflow_;
		T len_; // byte left in the buffer
	protected:
		T TOP_;
		const T size_;
		void set_start(T s) { start_ = s; };
		void set_len(T l) { len_ = l; };
	public:
		// debugging methods
		T size() const { return size_; };
		T len() const { return len_; };
		bool overflow() const { return overflow_; };
		T index() const { return idx_; };
		T start() const { return start_; };
		T operator[](T const i) const { return buffer_[i]; };
		// contructor
		CBuffer(T size = CBUF_SIZE);
		void clear();
		bool popc(D*);
		T pop(D*, const T);
		bool push(D);
};

//! Clear the buffer.
template <typename T, typename D>
void CBuffer<T, D>::clear()
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
template <typename T, typename D>
CBuffer<T, D>::CBuffer(T sz) : size_{sz}
{
	buffer_ = std::make_unique<D[]>(size_);
	TOP_ = size_ - 1;
	clear();
}

/*! Extract a single object from the buffer.
 *
 * data = buffer[start]
 *
 * \param data the area where to copy the object.
 * \return true if ok
 * \warning possible race condition!
 */
template <typename T, typename D>
bool CBuffer<T, D>::popc(D *data)
{
	if (len_) {
		*data = buffer_[start_];

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
template <typename T, typename D>
T CBuffer<T, D>::pop(D* data, const T sizeofdata)
{
	T j {0};

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
 *  modified members data: overflow_, idx_, buffer_[idx_], len_
 */
template <typename T, typename D>
bool CBuffer<T, D>::push(D c)
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
#endif
