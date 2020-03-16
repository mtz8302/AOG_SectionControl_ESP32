// WIFI handling 16. Maerz 2020 for ESP32 and Nano 33 IoT -------------------------------------------


//here supporting 2 WiFi networks, so set number of network!!

void WiFi_Start_STA() {
  unsigned long timeout, timeout2;
#if HardwarePlatform == 0  //ESP32  
  WiFi.mode(WIFI_STA);   //  Workstation
#endif
  if (NetWorkNum == 2) {
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
          if (NetWorkNum == 2) { WiFi.begin(SCSet.ssid2, SCSet.password2); }
          else { WiFi.begin(SCSet.ssid, SCSet.password); }
          timeout2 = timeout + 300;
      }
      //WIFI LED blink in double time while connecting
      WiFi_LED_blink(2);
  }  //connected or timeout  
  
  Serial.println(""); //NL  
  if (WiFi.status() == WL_CONNECTED)
  {
      delay(200);  
      Serial.println();
      Serial.print("WiFi Client successfully connected to : ");
      if (NetWorkNum == 2) { Serial.println(SCSet.ssid2); } else { Serial.println(SCSet.ssid); }
      Serial.print("Connected IP - Address : ");
      IPAddress myIP = WiFi.localIP();
      Serial.println(myIP);
      IPAddress gwip = WiFi.gatewayIP();
      //after connecting get IP from router -> change it to x.x.x.IP Ending (from settings)
      if (myIP[3] != SCSet.myIPEnding) {
          myIP[3] = SCSet.myIPEnding; //set ESP32 IP to x.x.x.myIP_ending
          Serial.print("changing IP to: ");
          Serial.println(myIP);
#if HardwarePlatform == 0  //ESP32 
          if (!WiFi.config(myIP, gwip, SCSet.mask, gwip)) { Serial.println("STA Failed to configure"); }
#endif
#if HardwarePlatform == 1  //nano 33iot
          WiFi.config(myIP, gwip, gwip, SCSet.mask);
#endif
          delay(200);
          Serial.print("Connected IP - Address : ");
          myIP = WiFi.localIP();
      }
      SCSet.myip[0] = myIP[0];
      SCSet.myip[1] = myIP[1];
      SCSet.myip[2] = myIP[2];
      SCSet.myip[3] = myIP[3];
      Serial.println(myIP);
      Serial.print("Gateway IP - Address : ");
      Serial.println(gwip);
      SCSet.gwip[0] = gwip[0];
      SCSet.gwip[1] = gwip[1];
      SCSet.gwip[2] = gwip[2];
      SCSet.gwip[3] = gwip[3];
      SCSet.IPToAOG[0] =myIP[0];
      SCSet.IPToAOG[1] = myIP[1];
      SCSet.IPToAOG[2] = myIP[2];
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
  delay(5);
  WiFi.softAP(SCSet.ssid_ap, "");
  while (!SYSTEM_EVENT_AP_START) // wait until AP has started
   {
    delay(100);
    Serial.print(".");
   }   
  delay(200);//right IP adress only with this delay 
  WiFi.softAPConfig(SCSet.gwip, SCSet.gwip, SCSet.mask);  // set fix IP for AP  
  delay(300);//right IP adress only with this delay 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Accesspoint started - Name : ");
  Serial.println(SCSet.ssid_ap);
  Serial.print( " IP address: ");
  Serial.println(myIP);
  SCSet.IPToAOG[0] = myIP[0];
  SCSet.IPToAOG[1] = myIP[1];
  SCSet.IPToAOG[2] = myIP[2];
  SCSet.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network
  SCSet.myip[0] = myIP[0];
  SCSet.myip[1] = myIP[1];
  SCSet.myip[2] = myIP[2];
  SCSet.myip[3] = myIP[3];

  LED_WIFI_ON = true;
  digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
  my_WiFi_Mode = WIFI_AP;
}
#endif

// AccessPoint on Nano 33 IoT

#if HardwarePlatform == 1
void WiFi_Start_AP() {
    WiFi.end();
    delay(500);
    delay(300);

    WiFi.config(SCSet.gwip, SCSet.gwip, SCSet.gwip, SCSet.mask);// set fix IP for AP  
    delay(300);  //right IP adress only with this delay
    byte my_Wifi_mode = WiFi.beginAP(SCSet.ssid_ap);
    delay(500);
    delay(300);
    Serial.print("status of WiFi AP: ");
    Serial.println(my_Wifi_mode);
    IPAddress myip = WiFi.localIP();
    // delay(300);
    Serial.print("Accesspoint started - Name : ");
    Serial.println(SCSet.ssid_ap);
    Serial.print(" IP address: ");
    Serial.println(myip);
    SCSet.myip[0] = myip[0];
    SCSet.myip[1] = myip[1];
    SCSet.myip[2] = myip[2];
    SCSet.myip[3] = myip[3];
    SCSet.IPToAOG[0] = myip[0];
    SCSet.IPToAOG[1] = myip[1];
    SCSet.IPToAOG[2] = myip[2];
    SCSet.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network    

    myip = WiFi.gatewayIP();
    Serial.print("Gateway IP - Address : ");
    Serial.println(myip);
    SCSet.gwip[0] = myip[0];
    SCSet.gwip[1] = myip[1];
    SCSet.gwip[2] = myip[2];
    SCSet.gwip[3] = myip[3];
    delay(50);

    LED_WIFI_ON = true;
    digitalWrite(SCSet.LEDWiFi_PIN, SCSet.LEDWiFi_ON_Level);
    my_WiFi_Mode = 2;
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
        for (byte n = 0; n < 4; n++) {
            Serial.print(SCSet.myip[n]);
            Serial.print(".");
        }
        Serial.print(" on port: ");
        Serial.println(SCSet.PortFromAOG);
        UDP_running = true;
        //getAOGSteerData();
    }
}


//-------------------------------------------------------------------------------------------------


void WiFi_connection_check() {
    delay(5);
    if (WiFi.status() == WL_CONNECTED) {
        if (SectAuto) {
            //WIFI LED blink 8x faster while no new data
            WiFi_LED_blink(3);
#if HardwarePlatform == 1//nano 33iot
            pingResult = WiFi.ping(SCSet.gwip);
            delay(5);
            Serial.print("no Section control Data, ping to Gateway (ms): "); Serial.print(pingResult);
            Serial.print("   Watchdog counter: "); Serial.println(WiFiWatchDog);
            if (pingResult >= 0) { WiFiWatchDog = 0; }
            else WiFiWatchDog++;
        }
        if (WiFiWatchDog > 3) {//reconnect
            LED_WIFI_ON = false;
            digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
            Serial.print("WiFi error: no data for "); Serial.print(currentTime - DataFromAOGTime);
            Serial.print(" ms. No ping to "); for (byte n = 0; n < 4; n++) { Serial.print(SCSet.gwip[n]); Serial.print("."); }
            Serial.println();
            Serial.print("Closing WiFi and try to reconnect to network: ");
            if (NetWorkNum == 2) { Serial.println(SCSet.ssid2); }
            else { Serial.println(SCSet.ssid); }
            WiFi.end();
            delay(200);
            delay(200);
            timeout = millis() + 10000;//close wifi every 10s when no new connection
            WiFiWatchDog = 0;
#endif
        }
        if (WiFi.localIP()[3] != SCSet.myIPEnding) {
#if HardwarePlatform == 0  //ESP32 
            if (!WiFi.config(SCSet.myip, SCSet.gwip, SCSet.mask, SCSet.gwip)) { Serial.println("STA Failed to configure"); }
#endif
#if HardwarePlatform == 1//nano 33iot
            WiFi.config(SCSet.myip, SCSet.gwip, SCSet.gwip, SCSet.mask);
#endif
            delay(200);
            delay(5);
            Serial.println();
            Serial.print("WiFi Client connected to : ");
            if (NetWorkNum == 2) { Serial.println(SCSet.ssid2); }
            else { Serial.println(SCSet.ssid); }
            Serial.print("Connected IP - Address : ");
            Serial.println(WiFi.localIP());
            WiFiWatchDog = 0;
        }
        if (!UDP_running) { UDP_Start(); }
    }
    else {
        //Serial.print("Wifi Watchdog: "); Serial.println(WiFiWatchDog);
        UDP_running = false;
        digitalWrite(SCSet.LEDWiFi_PIN, !SCSet.LEDWiFi_ON_Level);
        LED_WIFI_ON = false;
        WiFiWatchDog++;
        if (WiFiWatchDog > 50) {
            WiFiWatchDog = 0;//give router some time to check request
            if (millis() > timeout) {
                Serial.println();
                Serial.print("Closing WiFi and try to reconnect to network: ");
                if (NetWorkNum == 2) { Serial.println(SCSet.ssid2); }
                else { Serial.println(SCSet.ssid); }
#if HardwarePlatform == 0  //ESP32  
                WiFi.disconnect();
#endif
#if HardwarePlatform == 1//nano 33iot
                WiFi.end();
#endif
                delay(200);
                delay(200);
#if HardwarePlatform == 0  //ESP32  
                WiFi.mode(WIFI_STA);   //  Workstation
#endif
                timeout = millis() + 10000;//close wifi every 10s when no new connection
            }
            //reconnection
            Serial.print(".");
            if (NetWorkNum == 2) { WiFi.begin(SCSet.ssid2, SCSet.password2); }
            else { WiFi.begin(SCSet.ssid, SCSet.password); }
            delay(10);
        }
    }
}
