void SetRelays(void)
{
	if (SCSet.SectNum > 8) {
		for (byte i = 0; i < 8; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				digitalWrite(SCSet.Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(SCSet.Relay_PIN[i], HIGH);
			}
		}
		for (byte i = 8; i < SCSet.SectNum; i++)
		{
			if (bitRead(RelayOUT[1], (i - 8)) == LOW) {
				digitalWrite(SCSet.Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(SCSet.Relay_PIN[i], HIGH);
			}
		}
		if (SCSet.debugmodeRelay) {
			Serial.print("numbSect: "); Serial.print(SCSet.SectNum);
			Serial.print("  byte Relays 16-8: "); Serial.print(RelayOUT[1]);
			Serial.print("  byte Relays 8-1: "); Serial.println(RelayOUT[0]);
		}
	}
	else
	{
		for (byte i = 0; i < SCSet.SectNum; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				digitalWrite(SCSet.Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(SCSet.Relay_PIN[i], HIGH);
			}
		}
		//Serial.println(RelayOUT[0]);
	}
}
