#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

#define SHIELD_CS 7
#define SHIELD_DCS 6
#define CARDCS 4
#define DREQ 3

struct mapping_entry {
	char tag[10];
	char track[13];
};

struct config {
	struct mapping_entry mapping[5];
};

static struct config config;

static Adafruit_VS1053_FilePlayer player =
	Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

static void mapping_read(struct mapping_entry *mapping, File *file, unsigned max_mappings)
{
	struct mapping_entry e;
	unsigned i, cnt;

	for (cnt = 0; cnt < max_mappings; cnt++) {
		char *tag = e.tag;
		char *track = e.track;
		char c;

		memset(&e, 0, sizeof(e));

		for (i = 0; i < sizeof(e.tag); i++) {
			c = file->read();
			if (c == -1)
				return;

			*tag++ = c;
		}

		c = file->read();
		if (c != ' ')
			return;

		for (i = 0; i < sizeof(e.track) - 1; i++) {
			c = file->peek();
			if (c == '\n' || c == -1)
				break;

			*track++ = file->read();
		}

		/* sanity check track */
		if (e.track[0] == 0)
			return;

		*mapping++ = e;

		/* swallow until newline */
		do {
			c = file->read();
		} while (c != '\n' && c != -1);
	}
}

static void config_dump(struct config *c)
{
	struct mapping_entry *map_e = c->mapping;
	int i;

	Serial.println("[*] mappings:");
	i = 0;
	while (*map_e->tag != 0) {
		p("[%d] %.10s -> %s", i, map_e->tag, map_e->track);
		map_e++;
		i++;
	}
}

void setup() {
	File f;

	Serial.begin(57600);
	Serial.println();
	Serial.println();

	player.begin();
	SD.begin(CARDCS);

	player.setVolume(60, 60);
	player.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
	player.playFullFile((char *)"startup.mp3");

	f = SD.open("mapping.txt", FILE_READ);
	if (f) {
		mapping_read(config.mapping, &f, 5);
		f.close();
	}

	config_dump(&config);
}

void loop() {
}
