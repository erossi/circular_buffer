# Copyright (C) 2015, 2016 Enrico Rossi
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc

# CBUF_SAFE_EOM put a \0 at the end of the buffer
# CBUF_SIZE the size of the buffer, default to 20
CFLAGS = -Wall -Wstrict-prototypes -pedantic

.PHONY: clean indent data char
.SILENT: help
.SUFFIXES: .c, .o

all: data char

data: circular_buffer.o
	gcc $(CFLAGS) -o test_data test_data.c circular_buffer.o

char:
	gcc $(CFLAGS) -D CBUF_OVR_CHAR=45 -c circular_buffer.c
	gcc $(CFLAGS) -o test_message test_message.c circular_buffer.o

clean:
	rm *.o test_message test_data
