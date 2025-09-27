#include "string.h"

bool str_append_char(char *buf, const char c, const int buf_size) {
	int size = str_length(buf);
	if(size < buf_size - 2) {
		buf[size] = c;
		buf[size+1] = 0;
		return true;
	}
	return false;
}

int str_length(const char *buf) {
	int size = 0;
	while(*buf) {
		size++;
		buf++;
	}
	return size;
}
