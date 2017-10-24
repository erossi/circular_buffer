# Copyright (C) 2017 Enrico Rossi
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

CXX = g++
CXXFLAGS = -Wall -pedantic -std=c++14

.PHONY: clean
.SILENT: help
.SUFFIXES: .c, .o

all:

test_buffer: circular_buffer.o
	$(CXX) $(CXXFLAGS) -o test_buffer test_buffer.cpp circular_buffer.o

test_message: circular_buffer_message.o circular_buffer.o
	$(CXX) $(CXXFLAGS) -o test_buffer test_buffer.cpp circular_buffer.o

clean:
	rm -f *.o test_message test_data test_record
