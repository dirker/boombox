#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

#define SHIELD_CS 7
#define SHIELD_DCS 6
#define CARDCS 4
#define DREQ 3

static int i;
Adafruit_VS1053_FilePlayer player =
	Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void setup() {
	Serial.begin(57600);

	player.begin();
	SD.begin(CARDCS);

	player.setVolume(60, 60);
	player.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
	player.playFullFile("startup.mp3");
}

void loop() {
	i++;
	Serial.println(i);
}
