void SetRelays(void)
{
	if (SectNum > 8) {
		for (byte i = 0; i < 8; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				digitalWrite(Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(Relay_PIN[i], HIGH);
			}
		}
		for (byte i = 7; i < SectNum; i++)
		{
			if (bitRead(RelayOUT[1], i) == LOW) {
				digitalWrite(Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(Relay_PIN[i], HIGH);
			}
		}
	}
	else
	{
		for (byte i = 0; i < SectNum; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				digitalWrite(Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(Relay_PIN[i], HIGH);
			}
		}
		//Serial.println(RelayOUT[0]);
	}
}
