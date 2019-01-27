//Core1: handle Relays, Motor/PWM, switches 

//UDP communication does core2

void Core1code(void * pvParameters) {
	for (;;)

		// MAIN LOOP FOR THIS CORE
	{
		//delay(0); //do WIFI stuff
		currentTime = millis();
		unsigned int time = currentTime;

		//timed loop
		if ((currentTime - lastTime >= LOOP_TIME) || SectSWpressed)
		{
			dT = currentTime - lastTime;
			lastTime = currentTime;

			//reset ISR Left Side
			countsThisLoopLeft = pulseCountLeft;
			pulseCountLeft = 0;

			if (countsThisLoopLeft)
			{
				//total pulse time over counts in microseconds
				pulseAverageLeft = pulseDurationLeft / countsThisLoopLeft;
				pulseDurationLeft = 0;
			}

			//Right Side
			countsThisLoopRight = pulseCountRight;
			pulseCountRight = 0;

			if (countsThisLoopRight)
			{
				//total pulse time over counts in microseconds
				pulseAverageRight = pulseDurationRight / countsThisLoopRight;
				pulseDurationRight = 0;
			}

			//increase the watchdog - reset in data recv.
			watchdogTimer++;

			//clean out serial buffer
			if (watchdogTimer > 12)
			{
				while (Serial.available() > 0) char t = Serial.read();
				watchdogTimer = 0;
			}
			

			//accumulated counts from this cycle
			accumulatedCountsLeft += countsThisLoopLeft;
			accumulatedCountsRight += countsThisLoopRight;

			//only if section(s) are on and there is flow
			if (countsThisLoopLeft) {    //what is current flowrate from meter
				rateKLeft = (float)pulseAverageLeft * 0.001;
				if (rateKLeft < .001) rateKLeft = 0.1;//prevent divide by zero      //
				else rateKLeft = ((1.0 / rateKLeft) * 60) / flowmeterCalFactorLeft;

				//Kalman filter
				PcLeft = PLeft + varProcessLeft;
				GLeft = PcLeft / (PcLeft + varRateLeft);
				PLeft = (1 - GLeft) * PcLeft; XpLeft = XeRateLeft; ZpLeft = XpLeft;
				XeRateLeft = GLeft * (rateKLeft - ZpLeft) + XpLeft;

				rateAppliedLPMLeft = XeRateLeft * 100; //fill in formatted rateApplied
			}
			else {
				rateKLeft = 0.1;
				rateAppliedLPMLeft = 0;
			}

			//only if section(s) are on and there is flow
			if (countsThisLoopRight) {      //what is current flowrate from meter
				rateKRight = (float)pulseAverageRight * 0.001;
				if (rateKRight < .001) rateKRight = 0.1;//prevent divide by zero    //
				else rateKRight = ((1.0 / rateKRight) * 60) / flowmeterCalFactorRight;

				//Kalman filter
				PcRight = PRight + varProcessRight;
				GRight = PcRight / (PcRight + varRateRight);
				PRight = (1 - GRight) * PcRight; XpRight = XeRateRight; ZpRight = XpRight;
				XeRateRight = GRight * (rateKRight - ZpRight) + XpRight;

				rateAppliedLPMRight = XeRateRight * 100; //fill in formatted rateApplied
			}
			else {
				rateKRight = 0.1;
				rateAppliedLPMRight = 0;
			}

			//Do the PID - this placed in code depending on valve style
			rateErrorLeft = rateSetPointLeft - rateKLeft;
			rateErrorRight = rateSetPointRight - rateKRight;

			calcRatePID();
			if (SCSet. RateControlLeftEquiped) { motorDrive(); }

			//Also needs right side TODO
			//calcRatePIDRight();
			//motorDriveRight();

			RelayToAOG[2] = rateAppliedLPMLeft; //100 x actual!;
			RelayToAOG[3] = rateAppliedLPMRight; //100 x actual!
			RelayToAOG[4] = (int)((float)accumulatedCountsLeft / flowmeterCalFactorLeft)
				+ (int)((float)accumulatedCountsRight / flowmeterCalFactorRight);
			//RelayToAOG 5+6 set in ReadSwitches_buildBytes HiByte, LowByte if bit set -> AOG section forced ON
			RelayToAOG[7] = SectSWOffToAOG[1]; //HiByte if bit set -> AOG section forced off
			RelayToAOG[8] = SectSWOffToAOG[0]; //LowByte if bit set -> AOG section forced off
			RelayToAOG[9] = SectMainToAOG; // Bits: AOG section control AUTO, Section control OFF, Rate L+R ...
			NewDataToAOG = true; //send UDP message

						 
			//print for debug
			Serial.print("Bytes to AOG: ");
			for (byte i = 0; i < 10; i++) {
				Serial.print(RelayToAOG[i]);
				Serial.print(",");
			}
			Serial.println();
			Serial.flush();   // flush out buffer
						
		} //end of timed loop

		//****************************************************************************************
		//This runs continuously, outside of the timed loop, keeps checking UART for new data
		// header high/low, relay byte, speed byte, rateSetPoint hi/lo

	
		if ((SCSet.SectSWequiped) | (SCSet.SectMainSWType != 0))
		{
			SectSWRead(); //checks if section switch is toggled
			//RelayToAOG 5+6 set in ReadSwitches_buildBytes HiByte, LowByte if bit set -> AOG section forced ON
			RelayToAOG[7] = SectSWOffToAOG[1]; //HiByte if bit set -> AOG section forced off
			RelayToAOG[8] = SectSWOffToAOG[0]; //LowByte if bit set -> AOG section forced off
			RelayToAOG[9] = SectMainToAOG; // Bits: AOG section control AUTO, Section control OFF, Rate L+R ...
		}
		else { RelayOUT[0] = RelayFromAOG[0]; RelayOUT[1] = RelayFromAOG[1]; }

		//RelayOUT[0] = RelayFromAOG[0]; RelayOUT[1] = RelayFromAOG[1];
		SetRelays();	
		
		if (SCSet. RateSWLeftEquiped | SCSet.RateSWRightEquiped) { RateSWRead(); }
		if (!SCSet. RateControlLeftEquiped) { motorDrive(); } //if Manual do everytime, not only in timed loop
	}//end of main
}

