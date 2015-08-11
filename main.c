/*
    Circular Buffer, a string oriented circular buffer implementation.
    Copyright (C) 2015 Enrico Rossi

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
#include "circular_buffer.h"

void printit(struct cbuffer_t *cbuffer)
{
	uint8_t i;

	/* Print */
	printf("\n");
	printf("idx  : %d\n", cbuffer->idx);
	printf("msgs : %d\n", cbuffer->msgs);
	printf("start: %d\n", cbuffer->start);
	printf("owfl : %d\n", cbuffer->FLowf);
	printf("\n");

	for (i=0; i < CBUF_SIZE; i++)
		printf(" %2d", i);

	printf("\n");

	for (i=0; i < CBUF_SIZE; i++)
		if (*(cbuffer->buffer + i))
			printf("  %c", *(cbuffer->buffer + i));
		else
			printf("  *");

	printf("\n");
	printf("\n");
}

int main(void) {
	struct cbuffer_t *cbuffer;
	char *message;
	uint8_t FLloop;
	char rxc;

	FLloop=TRUE;
	cbuffer = cbuffer_init();
	message = malloc(CBUF_SIZE);

	while (FLloop) {
		/* get the char */
		rxc = getchar();

		switch(rxc) {
			case 'g':
				if (cbuffer_getmsg(cbuffer, message))
					printf("\nMSG: %s\n", message);

				printit(cbuffer);
				break;
			case 'q':
				FLloop=FALSE;
				break;
			case '\n':
				break;
			default:
				if (rxc != '0')
					rxc = 'a' + cbuffer->idx;
				else
					rxc = 0;

				cbuffer_add(cbuffer, rxc);
				printit(cbuffer);
		}
	}

	free(message);
	cbuffer_shut(cbuffer);
	return(0);
}
