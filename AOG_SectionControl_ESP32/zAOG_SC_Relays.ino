void SetRelays(void)
{
	uint16_t PCA_val;
	if (Set.SectRelaysON == 1) PCA_val = 0; else PCA_val = 65535;

	if (Set.SectNum > 8) {
		for (byte i = 0; i < 8; i++)
		{
			if (Set.Relay_PIN[i] < 255) {
				if (Set.Relay_PIN[i] < 100) {
					if (bitRead(RelayOUT[0], i) == LOW) {
						digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON);
					}
					else {
						digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON);
					}
				}
				else {
					if (Set.SectRelaysON == 1) {
						if (bitRead(RelayOUT[0], i) == HIGH) bitSet(PCA_val, (Set.Relay_PIN[i] - 100));
					}
					else { if (bitRead(RelayOUT[0], i) == HIGH) bitClear(PCA_val, (Set.Relay_PIN[i] - 100)); }
				}
			}
		}
		for (byte i = 8; i < Set.SectNum; i++)
		{
			if (Set.Relay_PIN[i] < 255) {
				if (Set.Relay_PIN[i] < 100) {
					if (bitRead(RelayOUT[1], (i - 8)) == LOW) {
						digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON);
					}
					else {
						digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON);
					}
				}
				else {
					if (Set.SectRelaysON == 1) {
						if (bitRead(RelayOUT[1], i - 8) == HIGH) bitSet(PCA_val, (Set.Relay_PIN[i] - 100));
					}
					else { if (bitRead(RelayOUT[1], i - 8) == HIGH) bitClear(PCA_val, (Set.Relay_PIN[i] - 100)); }
				}
			}
		}
		if (Set.debugmodeRelay) {
			Serial.print("numbSect: "); Serial.print(Set.SectNum);
			Serial.print(" byte Rel16-8: "); Serial.print(RelayOUT[1]);
			Serial.print(" byte Rel 8-1: "); Serial.println(RelayOUT[0]);
		}
	}
	else
	{
		for (byte i = 0; i < Set.SectNum; i++)
		{
			if (Set.Relay_PIN[i] < 255) {
				if (Set.Relay_PIN[i] < 100) {
					if (bitRead(RelayOUT[0], i) == LOW) {
						digitalWrite(Set.Relay_PIN[i], !Set.SectRelaysON);
					}
					else {
						digitalWrite(Set.Relay_PIN[i], Set.SectRelaysON);
					}
				}
				else {
					if (Set.SectRelaysON == 1) {
						if (bitRead(RelayOUT[0], i) == HIGH) bitSet(PCA_val, (Set.Relay_PIN[i] - 100));
					}
					else { if (bitRead(RelayOUT[0], i) == HIGH) bitClear(PCA_val, (Set.Relay_PIN[i] - 100)); }
				}
			}
			//Serial.println(RelayOUT[0]);
		}
	}

	if (Set.Relais_MainValve_PIN < 100) {//main valve
		if ((RelayOUT[0] > 0) || (RelayOUT[1] > 0)) { digitalWrite(Set.Relais_MainValve_PIN, Set.SectRelaysON); }
		else { digitalWrite(Set.Relais_MainValve_PIN, !Set.SectRelaysON); }
	}
	else {
		if (Set.Relais_MainValve_PIN < 116) {//main valve PCA9555
			if ((RelayOUT[0] > 0) || (RelayOUT[1] > 0)) { bitSet(PCA_val, Set.Relais_MainValve_PIN - 100); }
		}
	}

	//write output to PCA9555
	if (PCAinUse) {
		if (Set.debugmodeRelay) { Serial.print("Relay PCA: "); Serial.println(PCA_val, BIN); }
		Wire.beginTransmission(Set.I2CAddrPCAOutput);
		Wire.write(0x02); // address port 0
		Wire.write(lowByte(PCA_val));  // value to send
	//	Wire.endTransmission();
	//	Wire.beginTransmission(0x21);
	//	Wire.write(0x03); // address port 1
		Wire.write(highByte(PCA_val));  // value to send
		Wire.endTransmission();
	}

}
