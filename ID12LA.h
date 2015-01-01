#ifndef __ID12LA_H
#define __ID12LA_H

#include <Stream.h>

class ID12LA
{
public:
	ID12LA();
	~ID12LA();

	void setup(Stream *s);

	bool update();
	char *get();
	void clear();

private:
	bool crc_ok();
	int hexchar(char c);

private:
	Stream *serial_;
	char tag_[10];
	char buf_[12];
	unsigned index_;
};

#endif
