/*
    Circular Buffer.
    Copyright (C) 2016 Enrico Rossi

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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "storage.h"

struct record_t {
	uint8_t id;
	char text;
};

void help(void)
{
	printf("\nUsage keys:\n");
	printf(" h : This help message.\n");
	printf(" g : Get shadow record from the buffer.\n");
	printf(" c : Commit the buffer.\n");
	printf(" C : Clear the buffer.\n");
	printf(" q : Quit.\n");
	printf(" CR : Do nothing.\n");
	printf(" <any others key> : Put a record in the buffer.\n");
}

void printit(struct storage_t *storage)
{
	printf("i[%d], ", storage->idx);
	printf("s[%d], ", storage->start);
	printf("S[%d], ", storage->shadow);
	printf("l[%d], ", storage->len);
	printf("L[%d], ", storage->shadow_len);
	printf("o[%d]\n", storage->flags.b.overflow);
}

int main(void) {
	struct storage_t *storage;
	struct record_t* record;
	uint8_t FLloop;
	char c;

	record = malloc(sizeof(struct record_t));
	storage = storage_init(sizeof(struct record_t));

	printf("\nTest record oriented circular buffer.\n");
	printf("Copyright (C) 2016 Enrico Rossi - GNU GPL\n");

	help();

	FLloop=TRUE;
	while (FLloop) {
		/* get the char */
		c = getchar();

		switch(c) {
			case 'g':
				if (storage_get(storage, record, FALSE)) {
					printf("> Record fetched: %d %c\n",
							record->id, record->text);
				} else {
					printf("> No data\n");
				}

				printit(storage);
				break;
			case 'h':
				help();
				break;
			case 'q':
				FLloop=FALSE;
				break;
			case 'c':
				storage_commit(storage);
				printit(storage);
				break;
			case 'C':
				storage_clear(storage);
				printit(storage);
				break;
			case '\n':
				break;
			default:
				record->id = storage->idx;
				record->text = 'a' + storage->idx;
				storage_push(storage, (void*)record);
				printit(storage);
		}
	}

	free(record);
	storage_shut(storage);
	return(0);
}
