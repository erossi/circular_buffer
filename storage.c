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

/*! \file storage.c
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "storage.h"

/*! Clear the buffer.
 */
void storage_clear(struct storage_t *storage)
{
	storage->idx = 0;
	storage->start = 0;
	storage->len = 0;
	storage->overflow = FALSE;
	storage->shadow = 0;
	storage->shadow_len = 0;
}

/*! return the record present in the buffer.
 */
uint8_t storage_len(struct storage_t *storage)
{
	return(storage->shadow_len);
}

/*! Initialize the buffer.
 *
 * \return the allocated struct.
 */
struct storage_t *storage_init(uint8_t record_size)
{
	struct storage_t *storage;

	storage = malloc(sizeof(struct storage_t));
	/*! the size of a single record in byte */
	storage->record_size = record_size;
	/*! the number of record available */
	storage->size = STORAGE_SIZE;
	storage->buffer = malloc(STORAGE_SIZE * storage->record_size);

	if (storage->buffer)
		storage->TOP = STORAGE_SIZE - 1;

	storage_clear(storage);
	return(storage);
}

/*! Remove the buffer.
 */
void storage_shut(struct storage_t *storage)
{
	free(storage->buffer);
	free(storage);
}

/*! Commit the idx to the shadow index.
 *
 * The duplication of the setting is necessary to avoid race condition
 * with the storage_push().
 * Until the clear of storage->overflow, the push() will not be able to
 * add anything.
 */
void storage_commit(struct storage_t *storage)
{
	if (storage->start != storage->shadow) {
		storage->start = storage->shadow;
		storage->len = storage->shadow_len;
		storage->overflow = FALSE;
	} else {
		if (storage->overflow) {
			storage->start = storage->shadow;
			storage->len = storage->shadow_len;
			storage->overflow = FALSE;
		}
	}
}

/*! Reset the shadow ptrs.
 *
 */
void storage_reset(struct storage_t *storage)
{
	storage->shadow = storage->start;
	storage->shadow_len = storage->len;
}

/*! Get a record from the storage.
 *
 * Fetch from the start_index.
 */
uint8_t storage_get(struct storage_t* storage, void* record, uint8_t commit)
{
	if (storage->shadow_len) {
		memcpy(record, storage->buffer + storage->shadow * storage->record_size,
				storage->record_size);

		if (storage->shadow == storage->TOP)
			storage->shadow = 0;
		else
			storage->shadow++;

		storage->shadow_len--;

		if (commit)
			storage_commit(storage);

		return(TRUE);
	} else {
		return(FALSE);
	}
}

/*! push a record to the storage.
 */
uint8_t storage_push(struct storage_t *storage, void* record)
{
	/* If the buffer is full (overflow flag)
	 * do nothing.
	 */
	if (storage->overflow) {
		return(FALSE);
	} else {
		/* catch overflow */
		if (storage->start) {
			/* idx next to start? */
			if (storage->idx == (storage->start - 1))
				storage->overflow = TRUE;
		} else {
			/* idx next to the end of the buffer? */
			if (storage->idx == storage->TOP)
				storage->overflow = TRUE;
		}

		memcpy(storage->buffer + storage->idx * storage->record_size,
				record, storage->record_size);

		if (storage->idx == storage->TOP)
			storage->idx = 0;
		else
			storage->idx++;

		storage->len++;
		storage->shadow_len++;
		return(TRUE);
	}
}
