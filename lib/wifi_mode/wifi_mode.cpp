#include <wifi_mode.h>
#include <ESP32Ping.h>

//declare important variables
Audio* audio_ptr = NULL;
String SSID;
String PSWD;
int wifi_timeout = 10; //try connecting to WiFi for 10s
void build_station_list();
const char* custom_URL;
String playing_station = "";
String display_station = "";

//define states
State state_disp_KXLU(&disp_KXLU_on_enter, NULL, NULL);
State state_disp_LAist(&disp_LAist_on_enter, NULL, NULL);
State state_disp_KCRWLive(&disp_KCRWLive_on_enter, NULL, NULL);
State state_disp_KCRWMusic(&disp_KCRWMusic_on_enter, NULL, NULL);
State state_disp_KRCC(&disp_KRCC_on_enter, NULL, NULL);
State state_disp_Custom(&disp_Custom_on_enter, NULL, NULL);

//entry functions
void disp_KXLU_on_enter()
{
    display_station = String(stations[0].first);
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_LAist_on_enter()
{
    display_station = String(stations[1].first);
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_KCRWLive_on_enter()
{
    display_station = String(stations[2].first);
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_KCRWMusic_on_enter()
{
    display_station = String(stations[3].first);
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_KRCC_on_enter()
{
    display_station = String(stations[4].first);
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_Custom_on_enter()
{
    display_station = String(stations[5].first);
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}



void wifi_config(){
    build_station_list();
    lcd.clear();
    lcd.print("Internet Mode");
    SSID = NVS.getString(NVS_SSID);
    PSWD = NVS.getString(NVS_PSWD);
    audio_ptr = new Audio;
    Serial.println("Connecting to WiFi");
    lcd.setCursor(0,1);
    lcd.print("Connecting to WiFi");
    WiFi.disconnect();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PSWD);
    int wifi_start_time = millis();
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println("...Connecting to WiFi");
      delay(500);
      if ( (millis() - wifi_start_time) > (wifi_timeout * 1000) ){
        Serial.println("Can't connect to WiFi!");
        lcd.clear();
        lcd.print("No Wifi!");
        lcd.setCursor(0,1);
        lcd.print("Booting Menu");
        force_menu_boot();
      }
    }
    Serial.println("Connected to network");
    bool ret = Ping.ping("www.google.com");
    if (ret == true){
      lcd.clear();
      lcd.print("Found Google");
      delay(500);
    }
    else{
      lcd.clear();
      lcd.print("Can't Find Google");
      delay(500);
      force_menu_boot();
    }
    audio_ptr->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio_ptr->setVolume(10);
    lcd.clear();

};

void wifi_loop(){

};

void build_station_list(){
    custom_URL = NVS.getString(NVS_STAT).c_str();
    stations.push_back({"Custom", custom_URL});
}

