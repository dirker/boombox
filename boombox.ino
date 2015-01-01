#include <stdarg.h>

#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

#include <SoftwareSerial.h>
#include "ID12LA.h"

#define SHIELD_CS 7
#define SHIELD_DCS 6
#define CARDCS 4
#define DREQ 3

const int pin_rfid_rx = 8;
const int pin_rfid_tx = 9; /* not connected, needed for SoftwareSerial */
const int pin_power = 10;

struct mapping_entry {
	char tag[10];
	char track[13];
};

static Adafruit_VS1053_FilePlayer player =
	Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

static void p(const char *fmt, ...)
{
	char buf[64];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	Serial.println(buf);
}

static void hexdump(const void *p, size_t size)
{
	const char *c = (char *)p;

	Serial.println("--- 8< snip ----");
	for (unsigned i = 0; i < size; i++) {
		Serial.println(*c++, HEX);
	}
	Serial.println("--- >8 snap ----");
}

static bool mapping_readentry(struct mapping_entry *e, File *file)
{
	char *tag = e->tag;
	char *track = e->track;
	unsigned i;
	char c;

	memset(e, 0, sizeof(*e));

	for (i = 0; i < sizeof(e->tag); i++) {
		c = file->read();
		if (c == -1)
			return false;

		*tag++ = c;
	}

	c = file->read();
	if (c != ' ')
		return false;

	for (i = 0; i < sizeof(e->track) - 1; i++) {
		c = file->peek();
		if (c == '\n' || c == -1)
			break;

		*track++ = file->read();
	}

	/* sanity check track */
	if (e->track[0] == 0)
		return false;

	/* swallow until newline/end */
	do {
		c = file->read();
	} while (c != '\n' && c != -1);

	return true;
}

static void mapping_read(struct mapping_entry *mapping, File *file, unsigned max_mappings)
{
	struct mapping_entry e;
	unsigned cnt;
	bool ok;

	for (cnt = 0; cnt < max_mappings; cnt++) {
		ok = mapping_readentry(&e, file);
		if (!ok)
			return;

		*mapping++ = e;
	}
}

static bool mapping_find(struct mapping_entry *e, const char *fname, const char *tag)
{
	bool ret = false;
	File file;

	file = SD.open(fname, FILE_READ);
	if (!file)
		return false;

	while (mapping_readentry(e, &file)) {
		p("[*] checking %.10s (%s)", e->tag, e->track);
		if (memcmp(e->tag, tag, sizeof(e->tag)) == 0) {
			ret = true;
			break;
		}
	}

	file.close();
	return ret;
}

static SoftwareSerial rfid_serial(pin_rfid_rx, pin_rfid_tx);
static ID12LA rfid;


void setup() {
	File f;

	pinMode(pin_power, OUTPUT);
	digitalWrite(pin_power, HIGH);

	pinMode(pin_pause, INPUT);

	Serial.begin(57600);
	Serial.println();
	Serial.println();

	player.begin();

	SD.begin(CARDCS);

	player.setVolume(50, 50);
	player.playFullFile((char *)"_sys/startup.mp3");

	rfid_serial.begin(9600);
	rfid.setup(&rfid_serial);
}

static bool playing_track = false;

void loop() {
	char *tag = NULL;

	if (rfid.update()) {
		tag = rfid.get();
		p("[!] new tag: %.10s", tag);
	}

	if (tag) {
		struct mapping_entry e;
		bool ok;

		ok = mapping_find(&e, "_sys/mapping.txt", tag);
		if (ok) {
			p("[*] start playing %s", e.track);
			playing_track = true;
			ok = player.startPlayingFile(e.track);
			if (!ok)
				p("WTF!");
		}
	}

	player.feedBuffer();

	/* shutdown if sitting idle for too long */
	if (millis() > 30000 && !playing_track)
		digitalWrite(pin_power, LOW);

	/* shutdown if finished playing */
	if (playing_track && !player.playingMusic)
		digitalWrite(pin_power, LOW);
}
