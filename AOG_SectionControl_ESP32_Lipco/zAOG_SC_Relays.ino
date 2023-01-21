void SetRelays(void)
{
	//write relay byte to mcp
	Wire.beginTransmission(0x21);
	Wire.write(0x12); // address port A
	Wire.write(RelayOUT[0]);  // value to send
	Wire.endTransmission();
	Wire.beginTransmission(0x21);
	Wire.write(0x13); // address port B
	Wire.write(RelayOUT[1]);  // value to send
	Wire.endTransmission();
	
	
	/*
	if (Set.SectNum > 8) {
		for (byte i = 0; i < 8; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON); }
				else { if (Set.Relay_PIN[i] < 255) { mcpForOutput.digitalWrite(Set.Relay_PIN[i] - 100, !Set.SectRelaysON); } }
			}
			else {
				if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON); }
				else { if (Set.Relay_PIN[i] < 255) { mcpForOutput.digitalWrite(Set.Relay_PIN[i] - 100, Set.SectRelaysON); } }
			}
		}
		for (byte i = 8; i < Set.SectNum; i++)
		{
			if (bitRead(RelayOUT[1], (i - 8)) == LOW) {
				if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON); }
				else { if (Set.Relay_PIN[i] < 255) { mcpForOutput.digitalWrite(Set.Relay_PIN[i] - 100, !Set.SectRelaysON); } }
			}
			else {
				if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON); }
				else { if (Set.Relay_PIN[i] < 255) { mcpForOutput.digitalWrite(Set.Relay_PIN[i] - 100, Set.SectRelaysON); } }
			}
		}
		if (Set.debugmodeRelay) {
			Serial.print("numbSect: "); Serial.print(Set.SectNum);
			Serial.print("  byte Relays 16-8: "); Serial.print(RelayOUT[1]);
			Serial.print("  byte Relays 8-1: "); Serial.println(RelayOUT[0]);
		}
	}
	else
	{
		for (byte i = 0; i < Set.SectNum; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON); }
				else { if (Set.Relay_PIN[i] < 255) { mcpForOutput.digitalWrite(Set.Relay_PIN[i] - 100, !Set.SectRelaysON); } }
			}
			else {
				if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON); }
				else { if (Set.Relay_PIN[i] < 255) { mcpForOutput.digitalWrite(Set.Relay_PIN[i] - 100, Set.SectRelaysON); } }
			}
		}

		/*
		if (bitRead(RelayOUT[0], i) == LOW) {
			if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON); }
			else { mcpForOutput.digitalWrite(i, !Set.SectRelaysON); }
		}
		else {
			if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON); }
			else { mcpForOutput.digitalWrite(i, Set.SectRelaysON); }
		}
		if (bitRead(RelayOUT[0], i) == LOW) {
			if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON); }
			else { mcpForOutput.digitalWrite(i, !Set.SectRelaysON); }
		}
		else {
			if (Set.Relay_PIN[i] < 100) { digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON); }
			else { mcpForOutput.digitalWrite(i, Set.SectRelaysON); }
		}*/

		//Serial.println(RelayOUT[0]);
//	}//<8 sect

/*
	if (Set.Relais_MainValve_PIN != 255) {//main valve
		if ((RelayOUT[0] > 0) || (RelayOUT[1] > 0)) {
			if (Set.Relais_MainValve_PIN < 100) { digitalWrite(Set.Relais_MainValve_PIN, Set.SectRelaysON); }
			else { mcpForOutput.digitalWrite(Set.Relais_MainValve_PIN - 100, Set.SectRelaysON); }
		}
		else {
			if (Set.Relais_MainValve_PIN < 100) { digitalWrite(Set.Relais_MainValve_PIN, !Set.SectRelaysON); }
			else { mcpForOutput.digitalWrite(Set.Relais_MainValve_PIN - 100, !Set.SectRelaysON); }
		}
	}*/
}
