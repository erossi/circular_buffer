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

# Templated tests
test_buffer:
	$(CXX) $(CXXFLAGS) -D CBUF_OVR_CHAR=46 -o test_buffer test_buffer.cpp

test_message:
	$(CXX) $(CXXFLAGS) -D CBUF_OVR_CHAR=46 -o test_message test_message.cpp

test_shadow:
	$(CXX) $(CXXFLAGS) -D CBUF_OVR_CHAR=46 -o test_shadow test_shadow.cpp

clean:
	rm -f *.o test_buffer test_message test_shadow
