/* Copyright (C) 2016 Enrico Rossi

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/*! \file storage.h
 *
 * an evolution of the cbuffer, record oriented.
 */

#ifndef _STORAGE_H
#define _STORAGE_H

/*! The block size in byte */
#define STORAGE_BLK_SIZE 8

#ifndef STORAGE_SIZE
#define STORAGE_SIZE 10
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

struct storage_t {
	/*! the size of a single record */
	uint8_t record_size;
	uint8_t shadow;
	uint8_t shadow_len;
	/*! index ptr
	 *
	 * [ | | | | | | | | | | | | | | | | | | | | | | | | ]
	 * ^-buffer  ^-start   ^-shadow            ^-idx     ^-TOP
	 *                     ^ --- shadow len ---^
	 *           ^------------ len ------------^
	 * ^---------------------- size ---------------------^
	 */
	uint8_t *buffer;
	uint8_t idx;
	uint8_t start;
	uint8_t TOP;
	/* size of the buffer */
	uint8_t size;
	/* how many records are in the buffer */
	uint8_t len;

	/* note about endianess, do not refer to
	 * flags.all without knowing the endianess.
	 * flags.b.overflow=1 -> flags.all = 0x01 BIG END
	 * flags.b.overflow=1 -> flags.all = 0x80 LITTLE END
	 */
	union {
		/* GNU GCC only */
		__extension__ struct {

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
			/* lsb bit 0 */
			uint8_t overflow:1;
			uint8_t unused:7;
#else
			/* msb */
			uint8_t unused:7;
			uint8_t overflow:1;
#endif

		} b;

		uint8_t all;
	} flags;
};

void storage_clear(struct storage_t *storage);
struct storage_t* storage_init(uint8_t record_size);
void storage_shut(struct storage_t *storage);
uint8_t storage_get(struct storage_t *storage, void* record, uint8_t commit);
uint8_t storage_push(struct storage_t *storage, void* record);
void storage_commit(struct storage_t *storage);

#endif
