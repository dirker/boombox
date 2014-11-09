BOARD_TAG = uno

ARDUINO_LIBS += SPI
ARDUINO_LIBS += SD
ARDUINO_LIBS += SoftwareSerial

USER_LIB_PATH = vendor
ARDUINO_LIBS += adafruit-vs1053

include vendor/arduino-makefile/Arduino.mk
