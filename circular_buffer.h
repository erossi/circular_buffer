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

#include <stdint.h>

/*
 * Mandatory defs:
 * CBUF_SIZE
 *
 * Optional:
 * CBUF_OVR_CHAR
 *
 */
#ifndef CBUFFER_H
#define CBUFFER_H

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

// C++
#ifdef __cplusplus

#include <memory>

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

#else // __cplusplus

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

struct cbuffer_t {
	uint8_t *buffer;
	uint8_t idx;
	uint8_t start;
	uint8_t TOP;
	/* size of the buffer */
	uint8_t size;
	/* how many byte are in the buffer */
	uint8_t len;

	/* note about endianess, do not refer to
	 * flags without knowing the endianess.
	 * overflow=1 -> flags = 0x01 BIG END
	 * overflow=1 -> flags = 0x80 LITTLE END
	 */
	union {
		/* GNU c11 */
		struct {

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			/* lsb bit 0 */
			uint8_t overflow:1;
			uint8_t unused:7;
#else
			/* msb */
			uint8_t unused:7;
			uint8_t overflow:1;
#endif

		};

		uint8_t flags;
	};
};

void cbuffer_clear(struct cbuffer_t *cbuffer);
struct cbuffer_t *cbuffer_init(void);
void cbuffer_shut(struct cbuffer_t *cbuffer);
uint8_t cbuffer_pop(struct cbuffer_t *cbuffer, uint8_t * data,
		    const uint8_t size);
uint8_t cbuffer_popm(struct cbuffer_t *cbuffer, uint8_t * data,
		     const uint8_t size, const uint8_t eom);
uint8_t cbuffer_push(struct cbuffer_t *cbuffer, char rxc);

#endif // __cplusplus
#endif
