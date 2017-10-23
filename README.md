# 8-bit wide circular_buffer

Targeting small embedded device these functions implement
both string and binary oriented circular buffer.

# C++ compile test_message.cpp with:
g++ -D EOM=88 -D CBUF_OVR_CHAR=46 -g -o test_message test_message.cpp circular_buffer.cpp

