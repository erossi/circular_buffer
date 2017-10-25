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

#include <iostream>
#include <cstdio>
#include "circular_buffer.h"

#define MSG_SIZE 10 // message size less than buffer!
#define BUF_SIZE 15 // buffer size

using namespace std;

void help()
{
	cout << "Usage keys:" << endl;
	cout << " h : This help message." << endl;
	cout << " a : Get ALL data from the buffer." << endl;
	cout << " g : Get next object from the buffer." << endl;
	cout << " c : Clear the buffer." << endl;
	cout << " q : Quit." << endl;
	cout << " CR : Do nothing." << endl;
	cout << " <any others key> : Put a char in the buffer." << endl;
	cout << endl;
	cout << " \".\"   : buffer's slot empty." << endl;
	cout << " [1..f]: slot used." << endl;
	cout <<  endl;
}

// Print the content of the buffer and indexes.
// \note: cout << unsigned char does not print numbers.
void printit(const CBuffer<uint8_t, uint8_t>& cbuffer)
{
	printf("\n");
	printf("i: %d | ", cbuffer.index());
	printf("s: %d | ", cbuffer.start());
	printf("l: %d | ", cbuffer.len());
	printf("o: %d\n", cbuffer.overflow());
	printf("\n");

	for (uint8_t i = 0; i < cbuffer.size(); i++) {
		if (cbuffer.index() < cbuffer.start()) {
			if ((i >= cbuffer.start()) || (i < cbuffer.index()))
				printf("%1x", i);
			else
				printf(".");
		} else if (cbuffer.index() > cbuffer.start()) {
			if ((i >= cbuffer.start()) && (i < cbuffer.index()))
				printf("%1x", i);
			else
				printf(".");
		} else {
			if (cbuffer.len())
				printf("%1x", i);
			else
				printf(".");
		}
	}

	printf("\n");
}

int main() {
	CBuffer<uint8_t, uint8_t> cbuffer {BUF_SIZE};
	uint8_t *message;
	uint8_t len;
	bool FLloop {true};
	uint8_t rxc;

	message = new uint8_t[MSG_SIZE];
	cout << endl << "Test circular buffer." << endl;
	cout << "Copyright (C) 2015-2017 Enrico Rossi - GNU GPL" << endl;
	cout << endl << "Fill the buffer with chars from a to e and" << endl;
	cout << "get them one by one or all at once." << endl << endl;

	help();

	while (FLloop) {
		/* get the char */
		rxc = (uint8_t)getchar();

		switch(rxc) {
			case 'a':
				len = cbuffer.pop(message, MSG_SIZE);

				if (len) {
					cout << "> Objects fetched: ";
					cout << (int)len << " [";

					for (auto i = 0; i < len; i++)
						cout << *(message + i);

					cout << "]" << endl;
				} else {
					cout << "> No data" << endl;
				}

				printit(cbuffer);
				break;
			case 'g':
				if (cbuffer.popc(message)) {
					cout << "> Single object fetched: ";
					cout << message[0] << endl;
				} else {
					cout << "> No data." << endl;
				}

				printit(cbuffer);
				break;
			case 'h':
				help();
				break;
			case 'q':
				FLloop = false;
				break;
			case 'c':
				cbuffer.clear();
				printit(cbuffer);
				break;
			case '\n':
				break;
			default:
				rxc = 'a' + cbuffer.index();
				cbuffer.push(rxc);
				printit(cbuffer);
		}
	}

	delete[] message;

	return(0);
}
