# AOG_ESP32_Section_control
Section Control for AgOpenGPS for the ESP32


Section Control running on ESP32 for AgOpenGPS PC Software 


All switches have internal pullup, so switch to GND

GPIO 39 has no internal pullup, so external resistor 10k to 3.3V and switch to GND

Pressur(Flowrate) and Main Switch are (ON)-OFF-(ON) switches or push buttons: 3.3V - 1.15V - GND to get 1.15V use 10k to 3.3 and 10k to GND


For the outputs use a driver like UDN2981(relais have common GND and UDN does +) or ULN2803 (relais have common + and ULN does GND) 

make shure, that the SSID and password fits to your tractor WIFI
