#ifndef __ID12LA_H
#define __ID12LA_H

#include <SoftwareSerial.h>

class ID12LA
{
public:
	ID12LA(unsigned rx, unsigned tx);
	~ID12LA();

	void setup();

	bool update();
	char *get();
	void clear();

private:
	bool crc_ok();
	int hexchar(char c);

private:
	SoftwareSerial serial_;
	char tag_[10];
	char buf_[12];
	unsigned index_;
};

#endif
