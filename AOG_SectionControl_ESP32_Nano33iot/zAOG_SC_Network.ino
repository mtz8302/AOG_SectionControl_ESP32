// WIFI handling 1. März 2020 for ESP32 and Nano 33 IoT -------------------------------------------


//here supporting 2 WiFi networks, so send number of network!!

void WiFi_Start_STA(byte NW) {
  unsigned long timeout, timeout2;
#if HardwarePlatform == 0  //ESP32  
  WiFi.mode(WIFI_STA);   //  Workstation
#endif
  if (NW == 2) {
	  WiFi.begin(SCSet.ssid2, SCSet.password2); 
      timeout = millis() + (SCSet.timeoutRouter2 * 1000); 
      timeout2 = timeout - (SCSet.timeoutRouter2 * 500);
      Serial.print("try to connect to WiFi: "); Serial.println(SCSet.ssid2);
  }
  else {
	  Serial.print("try to connect to WiFi: "); Serial.println(SCSet.ssid);
      WiFi.begin(SCSet.ssid, SCSet.password); 
      timeout = millis() + (SCSet.timeoutRouter * 1000); 
      timeout2 = timeout - (SCSet.timeoutRouter * 500);
  }
  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
    delay(300);
    Serial.print(".");
    if ((millis() > timeout2) && (WiFi.status() != WL_CONNECTED)) {
#if HardwarePlatform == 0  //ESP32  
        WiFi.disconnect();
#endif
#if HardwarePlatform == 1  //nano 33iot 
        WiFi.end();
#endif
        delay(200);
        if (NW == 2) {WiFi.begin(SCSet.ssid2, SCSet.password2);}
        else {WiFi.begin(SCSet.ssid, SCSet.password);}
        timeout2 = timeout + 100;
    }
    //WIFI LED blink in double time while connecting
    if (!LED_WIFI_ON) {
        if (millis() > (LED_WIFI_time + (LED_WIFI_pause >> 2))) 
          {
           LED_WIFI_time = millis();
           LED_WIFI_ON = true;
           digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
          }
    }
    if (LED_WIFI_ON) {
      if (millis() > (LED_WIFI_time + (LED_WIFI_pulse >> 2))) {
        LED_WIFI_time = millis();
        LED_WIFI_ON = false;
        digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
      }
    }
  }  //connected or timeout  
  
  Serial.println(""); //NL  
  if (WiFi.status() == WL_CONNECTED)
  {
      delay(200);  
      Serial.println();
      Serial.print("WiFi Client successfully connected to : ");
      if (NW == 2) { Serial.println(SCSet.ssid2); } else { Serial.println(SCSet.ssid); }
      Serial.print("Connected IP - Address : ");
      IPAddress myip = WiFi.localIP();
      Serial.println(myip);
      IPAddress gwip = WiFi.gatewayIP();
      //after connecting get IP from router -> change it to x.x.x.IP Ending (from settings)
      myip[3] = SCSet.myIPEnding; //set ESP32 IP to x.x.x.myIP_ending
      Serial.print("changing IP to: ");
      Serial.println(myip);
#if HardwarePlatform == 0  //ESP32 
      if (!WiFi.config(myip, gwip, SCSet.mask, gwip)) { Serial.println("STA Failed to configure"); }
#endif
#if HardwarePlatform == 1  //nano 33iot
      WiFi.config(myip, gwip, gwip, SCSet.mask );
#endif
      delay(200);
      Serial.print("Connected IP - Address : ");
      myip = WiFi.localIP();
      Serial.println(myip);
      Serial.print("Gateway IP - Address : ");
      Serial.println(gwip);
      SCSet.IPToAOG[0] =myip[0];
      SCSet.IPToAOG[1] = myip[1];
      SCSet.IPToAOG[2] = myip[2];
      SCSet.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network
      LED_WIFI_ON = true;
      digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
      my_WiFi_Mode = 1;// WIFI_STA;
  }
  else
  {
     // WiFi.end();
      Serial.println("WLAN-Client-Connection failed");
      Serial.println();
      LED_WIFI_ON = false;
      digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
  }
  delay(20);
}


//-------------------------------------------------------------------------------------------------
// start WiFi Access Point = only if no existing WiFi

//ESP32
#if HardwarePlatform == 0 
void WiFi_Start_AP() {
  WiFi.mode(WIFI_AP);   // Accesspoint
  WiFi.softAP(SCSet.ssid_ap, "");
  while (!SYSTEM_EVENT_AP_START) // wait until AP has started
   {
    delay(100);
    Serial.print(".");
   }   
  delay(100);//right IP adress only with this delay 
  WiFi.softAPConfig(SCSet.gwip, SCSet.gwip, SCSet.mask);  // set fix IP for AP  

  IPAddress getmyIP = WiFi.softAPIP();
  delay(300);

  //AP_time = millis();
  Serial.print("Accesspoint started - Name : ");
  Serial.println(SCSet.ssid_ap);
  Serial.print( " IP address: ");
  Serial.println(getmyIP);
  LED_WIFI_ON = true;
  digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
  my_WiFi_Mode = WIFI_AP;
}
#endif

// AccessPoint on Nano 33 IoT

#if HardwarePlatform == 1
void WiFi_Start_AP() {

    my_WiFi_Mode = WiFi.beginAP(SCSet.ssid_ap, "");
    delay(500);
    if (my_WiFi_Mode != WL_AP_LISTENING) {
        Serial.println("Creating access point failed");
        delay(500);
        WiFi.end();
        delay(500);
        my_WiFi_Mode = WiFi.beginAP(SCSet.ssid_ap, "");
        if (my_WiFi_Mode != WL_AP_LISTENING) {
            Serial.println("Creating access point failed");
        }
    }
    delay(300);//right IP adress only with this delay 
    WiFi.config(SCSet.gwip, SCSet.gwip, SCSet.gwip, SCSet.mask);  // set fix IP for AP  
    delay(300); 
    IPAddress getmyIP = WiFi.localIP();
    delay(300);

    //AP_time = millis();
    Serial.print("Accesspoint started - Name : ");
    Serial.println(SCSet.ssid_ap);
    Serial.print(" IP address: ");
    Serial.println(getmyIP);
    LED_WIFI_ON = true;
    digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
    my_WiFi_Mode = WiFi.status();
}

#endif

//-------------------------------------------------------------------------------------------------

void UDP_Start()
{   //init UPD
    if (UDPToAOG.begin(SCSet.PortSCToAOG))
    {
        Serial.print("UDP sendig to IP: ");
        for (byte n = 0; n < 4; n++) {
            Serial.print(SCSet.IPToAOG[n]);
            Serial.print(".");
        }
        Serial.print(" from port: ");
        Serial.print(SCSet.PortSCToAOG);
        Serial.print(" to port: ");
        Serial.println(SCSet.portDestination);
    }
    delay(300);
    if (UDPFromAOG.begin(SCSet.PortFromAOG))
    {
        Serial.print("UDP listening for AOG data on IP: ");
        Serial.println(WiFi.localIP());
        Serial.print(" on port: ");
        Serial.println(SCSet.PortFromAOG);
        //getAOGSteerData();
    }
}





/* if (UDPFromAOG.begin(SCSet.PortFromAOG))
    {
        Serial.print("Section Control UDP Listening to port: ");
        Serial.println(SCSet.PortFromAOG);
        Serial.println();
    }
    delay(50);
    if (UDPToAOG.begin(SCSet.PortSCToAOG))
    {
        Serial.print("UDP writing to IP: ");
        Serial.print(SCSet.IPToAOG[0]); Serial.print("."); Serial.print(SCSet.IPToAOG[1]);  Serial.print("."); Serial.print(SCSet.IPToAOG[2]);  Serial.print("."); Serial.println(SCSet.IPToAOG[3]);
        Serial.print("UDP writing to port: ");
        Serial.println(SCSet.portDestination);
        Serial.print("UDP writing from port: ");
        Serial.println(SCSet.PortSCToAOG);
    }
}*/