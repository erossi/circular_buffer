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
#include "circular_buffer.h"

/* message size */
#define MSG_SIZE 16

void help(void)
{
	printf("\nUsage keys:\n");
	printf(" h : This help message.\n");
	printf(" g : Get raw data from the buffer.\n");
	printf(" c : Clear the buffer.\n");
	printf(" q : Quit.\n");
	printf(" CR : Do nothing.\n");
	printf(" <any others key> : Put a char in the buffer.\n");
	printf("\n");
}

void printit(struct cbuffer_t *cbuffer)
{
	uint8_t i;

	/* Print */
	printf("\n");
	printf("i: %d | ", cbuffer->idx);
	printf("s: %d | ", cbuffer->start);
	printf("o: %d\n", cbuffer->flags.b.overflow);
	printf("\n");

	for (i=0; i < CBUF_SIZE; i++) {
		if (cbuffer->idx < cbuffer->start) {
			if ((i < cbuffer->idx) || (i >= cbuffer->start))
				printf("%c", *(cbuffer->buffer + i));
			else
				printf("-");
		}

		if (cbuffer->idx > cbuffer->start) {
			if ((i >= cbuffer->start) && (i < cbuffer->idx))
				printf("%c", *(cbuffer->buffer + i));
			else
				printf("-");
		}

		if (cbuffer->idx == cbuffer->start) {
			if (cbuffer->flags.b.overflow)
				printf("%c", *(cbuffer->buffer + i));
			else
				printf("-");
		}
	}

	printf("\n");
}

int main(void) {
	struct cbuffer_t *cbuffer;
	uint8_t *message;
	uint8_t FLloop, len, i;
	char rxc;

	FLloop=TRUE;
	cbuffer = cbuffer_init();
	message = malloc(MSG_SIZE);

	printf("\nTest circular buffer.\n");
	printf("Copyright (C) 2016 Enrico Rossi - GNU GPL\n");

	help();

	while (FLloop) {
		/* get the char */
		rxc = getchar();

		switch(rxc) {
			case 'g':
				len = cbuffer_pop(cbuffer, message, MSG_SIZE);

				if (len) {
					printf("> Data fetched: %d\n", len);

					for (i=0; i < len; i++)
						printf("%c", *(message + i));

					printf("\n");
				} else {
					printf("> No data\n");
				}

				printit(cbuffer);
				break;
			case 'h':
				help();
				break;
			case 'q':
				FLloop=FALSE;
				break;
			case 'c':
				cbuffer_clear(cbuffer);
				printit(cbuffer);
				break;
			case '\n':
				break;
			default:
				rxc = 'a' + cbuffer->idx;
				cbuffer_push(cbuffer, rxc);
				printit(cbuffer);
		}
	}

	free(message);
	cbuffer_shut(cbuffer);
	return(0);
}
