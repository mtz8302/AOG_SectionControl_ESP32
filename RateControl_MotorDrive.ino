void motorDrive(void)
{
	if (SCSet. RateControlLeftEquiped) {
	//if (pwmDrive > 0) bitSet(PORTB, 4);  //set the correct direction
	//analogWrite(SCSet.FlowPWMLeft_PIN, pwmDrive);
	}
	else 
	//manual motor drive
	{
		if (!SCSet.RateControlPWM) 
		{	//if rate switch is toggled -> set Output high
			if (RateManUpLeft) { digitalWrite(SCSet.FlowDirLeft_PIN, HIGH); digitalWrite(SCSet.FlowPWMLeft_PIN, LOW); }
			if (RateManDownLeft) { digitalWrite(SCSet.FlowDirLeft_PIN, LOW); digitalWrite(SCSet.FlowPWMLeft_PIN, HIGH); }
			if (RateManUpRight) { digitalWrite(SCSet.FlowDirRight_PIN, HIGH); digitalWrite(SCSet.FlowPWMRight_PIN, LOW); }
			if (RateManDownRight) { digitalWrite(SCSet.FlowDirRight_PIN, LOW); digitalWrite(SCSet.FlowPWMRight_PIN, HIGH); }
			//toggle switch not pressed = set all low
			if (!(RateManUpLeft | RateManDownLeft | RateManUpRight | RateManDownRight))
				{ digitalWrite(SCSet.FlowDirRight_PIN, LOW); digitalWrite(SCSet.FlowPWMRight_PIN, LOW); digitalWrite(SCSet.FlowDirLeft_PIN, LOW); digitalWrite(SCSet.FlowPWMLeft_PIN, LOW);
				}
		}
	}
}
