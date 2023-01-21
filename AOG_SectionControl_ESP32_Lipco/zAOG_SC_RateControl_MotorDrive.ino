void motorDrive(void)
{/*
	if (Set.RateControlLeftInst == 1) {
		//if (pwmDrive > 0) bitSet(PORTB, 4);  //set the correct direction
		//analogWrite(Set.FlowPWMLeft_PIN, pwmDrive);
	}
	else
		//manual motor drive
	{

		if (Set.Relais_MainValve_PIN < 100) { digitalWrite(Set.Relais_MainValve_PIN, Set.SectRelaysON); }
		//else { mcpForOutput.digitalWrite(Set.Relais_MainValve_PIN - 100, Set.SectRelaysON); }

		if (Set.RateControlPWM == 0)
		{	//if rate switch is toggled -> set Output high
			if (!Set.MCPInUse) {
				//WEder PCB	no MCP23017		
				if (RateManUpLeft) {
					digitalWrite(Set.FlowDirLeft_PIN, HIGH); digitalWrite(Set.FlowPWMLeft_PIN, HIGH);
					if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate left manual up"); }
				}
				if (RateManDownLeft) {
					digitalWrite(Set.FlowDirLeft_PIN, LOW); digitalWrite(Set.FlowPWMLeft_PIN, HIGH);
					if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate left manual down"); }
				}
				if (RateManUpRight) { digitalWrite(Set.FlowDirRight_PIN, HIGH); digitalWrite(Set.FlowPWMRight_PIN, HIGH); }
				if (RateManDownRight) { digitalWrite(Set.FlowDirRight_PIN, LOW); digitalWrite(Set.FlowPWMRight_PIN, HIGH); }
				//toggle switch not pressed = set all low
				if (!(RateManUpLeft || RateManDownLeft || RateManUpRight || RateManDownRight))
				{
					digitalWrite(Set.FlowDirRight_PIN, LOW); digitalWrite(Set.FlowPWMRight_PIN, LOW); digitalWrite(Set.FlowDirLeft_PIN, LOW); digitalWrite(Set.FlowPWMLeft_PIN, LOW);
				}
			}
			else {

			*/
				//MCP
				//Lipco mod: write into output relay bytes
				if (RateManUpLeft) {
					bitSet(RelayOUT[1], 5); bitClear(RelayOUT[1], 4);
					if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate front manual up"); }
				}
				if (RateManDownLeft) {
					bitSet(RelayOUT[1], 4); bitClear(RelayOUT[1], 5);
					if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate front manual down"); }
				}
				if (RateManUpRight) { bitSet(RelayOUT[1], 7); bitClear(RelayOUT[1], 6);
				if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate rear manual up"); }
				}
				if (RateManDownRight) { bitSet(RelayOUT[1], 6); bitClear(RelayOUT[1], 7); 
				if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate rear manual down"); }
				}
				//toggle switch not pressed = set all low
				if (!(RateManUpLeft || RateManDownLeft || RateManUpRight || RateManDownRight))
				{
					bitClear(RelayOUT[1], 4); bitClear(RelayOUT[1], 5); bitClear(RelayOUT[1], 6); bitClear(RelayOUT[1], 7);
				}



				/*if ((Set.FlowDirLeft_PIN < 255) && (Set.FlowPWMLeft_PIN < 255)) {
					if (RateManUpLeft) {
						mcpForOutput.digitalWrite(Set.FlowDirLeft_PIN - 100, !Set.SectRelaysON);
						mcpForOutput.digitalWrite(Set.FlowPWMLeft_PIN - 100, Set.SectRelaysON);
						if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate left manual up"); }
					}
					if (RateManDownLeft) {
						mcpForOutput.digitalWrite(Set.FlowDirLeft_PIN - 100, Set.SectRelaysON);
						mcpForOutput.digitalWrite(Set.FlowPWMLeft_PIN - 100, !Set.SectRelaysON);
						if (Set.debugmode || Set.debugmodeRelay) { Serial.println("Rate left manual down"); }
					}
					//toggle switch not pressed = set all low
					if (!(RateManUpLeft || RateManDownLeft))
					{
						mcpForOutput.digitalWrite(Set.FlowDirLeft_PIN - 100, !Set.SectRelaysON);
						mcpForOutput.digitalWrite(Set.FlowPWMLeft_PIN - 100, !Set.SectRelaysON);
					}
				}
				if ((Set.FlowDirRight_PIN < 255) && (Set.FlowPWMRight_PIN < 255)) {
					if (RateManUpRight) {
						mcpForOutput.digitalWrite(Set.FlowDirRight_PIN - 100, !Set.SectRelaysON);
						mcpForOutput.digitalWrite(Set.FlowPWMRight_PIN - 100, Set.SectRelaysON);
					}
					if (RateManDownRight) {
						mcpForOutput.digitalWrite(Set.FlowDirRight_PIN - 100, Set.SectRelaysON);
						mcpForOutput.digitalWrite(Set.FlowPWMRight_PIN - 100, !Set.SectRelaysON);
					}


					//toggle switch not pressed = set all low
					if (!(RateManUpRight || RateManDownRight))
					{
						mcpForOutput.digitalWrite(Set.FlowDirRight_PIN - 100, !Set.SectRelaysON);
						mcpForOutput.digitalWrite(Set.FlowPWMRight_PIN - 100, !Set.SectRelaysON);
					}
				}
			}//else MCP
			*/
//		}
//	}
}
