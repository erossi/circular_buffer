/*
    Circular Buffer, a string oriented circular buffer implementation.
    Copyright (C) 2015, 2016 Enrico Rossi

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
	printf(" b : Get all data from the buffer.\n");
	printf(" g : Get next message from the buffer.\n");
	printf(" c : Clear the buffer.\n");
	printf(" 0 : Insert an EndOfMessage (\\0).\n");
	printf(" q : Quit.\n");
	printf(" CR : Do nothing.\n");
	printf(" <any others key> : Put a char in the buffer.\n");
	printf("\n");
	printf("note: stored chars start from letter 'a'.\n");
	printf("\n");
}

void printit(struct cbuffer_t *cbuffer)
{
	uint8_t i;

	/* Print */
	printf("\n");
	printf("i: %d | ", cbuffer->idx);
	printf("m: %d | ", cbuffer->flags.value.msgs);
	printf("s: %d | ", cbuffer->start);
	printf("o: %d\n", cbuffer->flags.value.overflow);
	printf("\n");

	for (i=0; i < CBUF_SIZE; i++)
		if (*(cbuffer->buffer + i))
			printf("%c", *(cbuffer->buffer + i));
		else
			printf("*");

	printf("\n");
}

int main(void) {
	struct cbuffer_t *cbuffer;
	char *message;
	uint8_t FLloop;
	char rxc;

	FLloop=TRUE;
	cbuffer = cbuffer_init(TRUE);
	message = malloc(MSG_SIZE);

	printf("\nTest circular buffer.\n");
	printf("Copyright (C) 2015, 2016 Enrico Rossi - GNU GPL\n");

	help();

	while (FLloop) {
		/* get the char */
		rxc = getchar();

		switch(rxc) {
			case 'g':
				if (cbuffer_popm(cbuffer, message, MSG_SIZE))
					printf("\nMSG: %s\n", message);

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
				if (rxc != '0')
					rxc = 'a' + cbuffer->idx;
				else
					rxc = 0;

				cbuffer_push(cbuffer, rxc);
				printit(cbuffer);
		}
	}

	free(message);
	cbuffer_shut(cbuffer);
	return(0);
}
