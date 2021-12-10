/* Circular Buffer, an object oriented circular buffer.
 * Copyright (C) 2015-2021 Enrico Rossi
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

#ifndef _CBUFFER_H_
#define _CBUFFER_H_

#include <memory>

#ifndef CBUF_SIZE // Default buffer size
#define CBUF_SIZE 16
#endif

/** Buffer structure

 [ | | | | | | | | | | | | | | | | | | | | | | | | ]
  ^buffer   ^start                        ^idx    ^TOP
            ^------------ len() ----------^
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
		virtual void pop_object(D*);
		virtual void push_object(D);
	protected:
		T TOP_;
		const T size_;
		void set_start(T s) { start_ = s; };
    void clear_overflow() { overflow_ = false; };
	public:
		// debugging methods
		T size() const { return size_; };
		bool overflow() const { return overflow_; };
		T index() const { return idx_; };
		T start() const { return start_; };
		// FIXME i < size_
		T operator[](T const i) const { return buffer_[i]; };
		// contructor
		CBuffer(T size = CBUF_SIZE);
		virtual void clear();
		virtual T len() const;
		bool popc(D*);
		T pop(D*, const T);
		T popm(D*, const T, const D);
		bool push(D);
};

//! Clear the buffer.
template <typename T, typename D>
void CBuffer<T, D>::clear()
{
	idx_ = 0;
	start_ = 0;
	overflow_ = false;
}

/** LENght of the buffer
 *
 * Calculate and return the len of the buffer, the number of
 * objects remaining.
 *
 * @return len
 * @note const function does not change any attribute.
 */
template <typename T, typename D>
T CBuffer<T, D>::len() const
{
  if (idx_ == start_) {
    if (overflow_)
      return size_;
    else
      return 0;
  } else if (idx_ > start_) {
    return (idx_ - start_);
  } else {
    return (size_ - start_ + idx_);
  }
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

/*! Get single object from the buffer.
 *
 * This perform the operation, which can be overridden
 * depending on the type of object.
 */
template <typename T, typename D>
void CBuffer<T, D>::pop_object(D *data)
{
	*data = buffer_[start_];
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
	if (len()) {
		pop_object(data); // override me

		if (start_ == TOP_)
			start_ = 0;
		else
			start_++;

		// From here possible race condition with push().
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
 * \warning if the sizeofdata is bigger than the allocated data,
 * it will segfault or worse.
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
T CBuffer<T, D>::popm(D* data, const T sizeofdata, const D eom)
{
	T j {0};

	// while there is space left on data
	//  and byte in the buffer
	//  and the data fetched is NOT EOM
	while ((j < sizeofdata) && this->popc(data + j) && (*(data + j) != eom))
		j++;

	return (j);
}

/*! Push the single object.
 *
 * This function should be overridden.
 */
template <typename T, typename D>
void CBuffer<T, D>::push_object(D c)
{
	buffer_[idx_] = c;
}

/*! add data to the buffer.
 *
 * \note if overflow and EOM then the last char must be the EOM.
 *
 * \warning race condition with other functions.
 *  modified members data: overflow_, idx_, buffer_[idx_]
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

		push_object(c); // override me

		if (idx_ == TOP_)
			idx_ = 0;
		else
			idx_++;

		return (true);
	}
}

#endif
