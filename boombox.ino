
static int i;

void setup() {
	Serial.begin(57600);
}

void loop() {
	i++;
	Serial.println(i);
}
