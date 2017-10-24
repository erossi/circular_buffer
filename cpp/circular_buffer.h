/*
    Circular Buffer, a string oriented circular buffer implementation.
    Copyright (C) 2015-2017 Enrico Rossi

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

#ifndef CBUFFER_H
#define CBUFFER_H

/*
 * Mandatory defs: CBUF_SIZE
 * Optional defs: CBUF_OVR_CHAR
 */

#include <stdint.h>
#include <memory>

#ifdef USE_DEFAULT_H
#include "default.h"
#endif

/*! The size of the buffer.
 *
 * default to 16
 */
#ifndef CBUF_SIZE
#define CBUF_SIZE 16
#endif

/*! Optional
 *
 * -D CBUF_OVR_CHAR='X'
 */

class CBuffer {
	private:
		// Fixed array size
		std::unique_ptr<uint8_t[]> buffer_;
		uint8_t idx_;
		uint8_t start_;
		uint8_t TOP_;
		bool overflow_;
		const uint8_t size_;
		uint8_t len_; // byte left in the buffer
		uint8_t bcpy(uint8_t*, const uint8_t, uint8_t);
	public:
		CBuffer(uint8_t size = CBUF_SIZE);
		void clear();
		uint8_t size() const { return size_; };
		uint8_t len() const { return len_; };
		bool overflow() const { return overflow_; };
		uint8_t index() const { return idx_; };
		uint8_t start() const { return start_; };
		uint8_t buffer(uint8_t const i) const { return buffer_[i]; };
		uint8_t pop(uint8_t*, const uint8_t);
		uint8_t popm(uint8_t*, const uint8_t, const uint8_t);
		bool push(char);
};

#endif
