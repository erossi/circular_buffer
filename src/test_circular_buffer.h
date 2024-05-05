/*
 * Circular Buffer, an object oriented circular buffer.
 * Copyright (C) 2015-2018 Enrico Rossi
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

// cxxtestgen --error-printer -o runner.cpp test_circular_buffer.h
// g++ -o runner runner.cpp
// ./runner

#include <cxxtest/TestSuite.h>
#include "circular_buffer.h"

class TestSuite1 : public CxxTest::TestSuite
{
	private:
		CBuffer<uint8_t, uint8_t> cbuffer {5};
		uint8_t datasize {5 + 1};
		uint8_t* data;

		void push(uint8_t v)
		{
			TS_ASSERT(cbuffer.push(v));
		}

	public:
		void setUp()
		{
			data = new uint8_t[datasize];

			cbuffer.clear();

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 0);
			TS_ASSERT_EQUALS(cbuffer.len(), 0);
			TS_ASSERT_EQUALS(cbuffer.index(), 0);
			TS_ASSERT(!cbuffer.overflow());
		}

		void tearDown()
		{
			delete[] data;
		}

		void testSingle(void)
		{
			push('a');
			TS_ASSERT_EQUALS(cbuffer[0], 'a');

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 0);
			TS_ASSERT_EQUALS(cbuffer.len(), 1);
			TS_ASSERT_EQUALS(cbuffer.index(), 1);
			TS_ASSERT(!cbuffer.overflow());

			TS_ASSERT(cbuffer.popc(data)); // popc
			TS_ASSERT_EQUALS('a', data[0]);

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 1);
			TS_ASSERT_EQUALS(cbuffer.len(), 0);
			TS_ASSERT_EQUALS(cbuffer.index(), 1);
			TS_ASSERT(!cbuffer.overflow());
		}

		void testMultiple(void)
		{
			// fill in 4 byte
			for (auto i = 0; i < 4; i++)
				push('a' + i);

			for (auto i = 0; i < 4; i++)
				TS_ASSERT_EQUALS(cbuffer[i], 'a' + i);

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 0);
			TS_ASSERT_EQUALS(cbuffer.len(), 4);
			TS_ASSERT_EQUALS(cbuffer.index(), 4);
			TS_ASSERT(!cbuffer.overflow());

			// Get 3 byte from the buffer
			TS_ASSERT_EQUALS(cbuffer.pop(data, 3), 3);

			for (auto i = 0; i < 3; i++)
				TS_ASSERT_EQUALS(data[i], 'a' + i);

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 3);
			TS_ASSERT_EQUALS(cbuffer.len(), 1);
			TS_ASSERT_EQUALS(cbuffer.index(), 4);
			TS_ASSERT(!cbuffer.overflow());

			// Get the last byte
			// check that got 1 byte when all datasize requested.
			TS_ASSERT_EQUALS(cbuffer.pop(data, datasize), 1);
			TS_ASSERT_EQUALS(data[0], 'd');

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 4);
			TS_ASSERT_EQUALS(cbuffer.len(), 0);
			TS_ASSERT_EQUALS(cbuffer.index(), 4);
			TS_ASSERT(!cbuffer.overflow());
		}

		void testOverflow(void)
		{
			// fill in
			for (auto i = 0; i < 5; i++) {
				push('a' + i);
				TS_ASSERT_EQUALS(cbuffer[i], 'a' + i);
			}

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 0);
			TS_ASSERT_EQUALS(cbuffer.len(), 5);
			TS_ASSERT_EQUALS(cbuffer.index(), 0);
			TS_ASSERT(cbuffer.overflow());

			TS_ASSERT(!cbuffer.push('x')); // fail to push

			// Get 3 byte from the buffer
			// [abc]
			TS_ASSERT_EQUALS(cbuffer.pop(data, 3), 3);

			for (auto i = 0; i < 3; i++)
				TS_ASSERT_EQUALS(data[i], 'a' + i);

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 3);
			TS_ASSERT_EQUALS(cbuffer.len(), 2);
			TS_ASSERT_EQUALS(cbuffer.index(), 0);
			TS_ASSERT(!cbuffer.overflow());

			// re-fill without overflow
			// [ab?de]
			for (auto i = 0; i < 2; i++)
				push('a' + i);

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 3);
			TS_ASSERT_EQUALS(cbuffer.len(), 4);
			TS_ASSERT_EQUALS(cbuffer.index(), 2);
			TS_ASSERT(!cbuffer.overflow());

			// Get all the buffer
			// [deab]
			TS_ASSERT_EQUALS(cbuffer.pop(data, datasize), 4);
			TS_ASSERT_EQUALS(data[0], 'd');
			TS_ASSERT_EQUALS(data[1], 'e');
			TS_ASSERT_EQUALS(data[2], 'a');
			TS_ASSERT_EQUALS(data[3], 'b');

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 2);
			TS_ASSERT_EQUALS(cbuffer.len(), 0);
			TS_ASSERT_EQUALS(cbuffer.index(), 2);
			TS_ASSERT(!cbuffer.overflow());

			// fill in
			// [deabc]
			for (auto i = 0; i < 5; i++)
				push('a' + i);

			TS_ASSERT_EQUALS(cbuffer[0], 'd');
			TS_ASSERT_EQUALS(cbuffer[1], 'e');
			TS_ASSERT_EQUALS(cbuffer[2], 'a');
			TS_ASSERT_EQUALS(cbuffer[3], 'b');
			TS_ASSERT_EQUALS(cbuffer[4], 'c');
			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 2);
			TS_ASSERT_EQUALS(cbuffer.len(), 5);
			TS_ASSERT_EQUALS(cbuffer.index(), 2);
			TS_ASSERT(cbuffer.overflow());

			TS_ASSERT(!cbuffer.push('x')); // fail to push

			// Get all the buffer
			// [deabc]
			TS_ASSERT_EQUALS(cbuffer.pop(data, datasize), 5);
			TS_ASSERT_EQUALS(data[0], 'a');
			TS_ASSERT_EQUALS(data[1], 'b');
			TS_ASSERT_EQUALS(data[2], 'c');
			TS_ASSERT_EQUALS(data[3], 'd');
			TS_ASSERT_EQUALS(data[4], 'e');

			TS_ASSERT_EQUALS(cbuffer.size(), 5);
			TS_ASSERT_EQUALS(cbuffer.start(), 2);
			TS_ASSERT_EQUALS(cbuffer.len(), 0);
			TS_ASSERT_EQUALS(cbuffer.index(), 2);
			TS_ASSERT(!cbuffer.overflow());
		}
};
