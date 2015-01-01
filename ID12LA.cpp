#include <Arduino.h>
#include "ID12LA.h"

ID12LA::ID12LA() :
  serial_(NULL)
{
}

ID12LA::~ID12LA()
{
}

void ID12LA::setup(Stream *s)
{
	serial_ = s;
}

bool ID12LA::update()
{
	bool updated = false;

	while (serial_->available()) {
		char c = serial_->read();

		/* start of tag */
		if (c == 0x02) {
			index_ = 0;
			continue;
		}

		/* ignore whitespace, stop */
		if (c == '\r' || c == '\n' || c == 0x03)
			continue;

		if (index_ >= sizeof(buf_)) {
			Serial.println("ID12LA fail");
			while (1);
		}

		buf_[index_++] = c;

		/* hit the end of the tag? check crc and copy tag */
		if (index_ == sizeof(buf_) && crc_ok()) {
			memcpy(tag_, buf_, sizeof(tag_));
			updated = true;
		}
	}

	return updated;
}

char *ID12LA::get()
{
	if (*tag_)
		return tag_;

	return NULL;
}

void ID12LA::clear()
{
	*tag_ = 0;
}

bool ID12LA::crc_ok()
{
	byte chk_ref = hexchar(buf_[10]) * 16 + hexchar(buf_[11]);
	byte chk = 0;

	for (int i = 0; i < 5; i++) {
		char *hex = &buf_[i*2];
		byte data = hexchar(hex[0]) * 16 + hexchar(hex[1]);

		chk ^= data;
	}
  
	return chk == chk_ref;
}

int ID12LA::hexchar(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return 0;
}
