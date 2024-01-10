#include <wifi_mode.h>
#include <ESP32Ping.h>
#include <vector>
#include <WiFi.h>
#include <Audio.h>
using namespace std;

//declare important variables
Audio* audio_ptr;
String SSID;
String PSWD;
int wifi_timeout = 10; //try connecting to WiFi for 10s
String custom_URL;
String playing_station;
String display_station;
int station_index = 0;
void build_station_list();
vector<pair<string, string>> stations;

//define states
//display
State state_disp_KXLU(&disp_KXLU_on_enter, NULL, NULL);
State state_disp_LAist(&disp_LAist_on_enter, NULL, NULL);
State state_disp_KCRWLive(&disp_KCRWLive_on_enter, NULL, NULL);
State state_disp_KCRWMusic(&disp_KCRWMusic_on_enter, NULL, NULL);
State state_disp_KRCC(&disp_KRCC_on_enter, NULL, NULL);
State state_disp_Custom(&disp_Custom_on_enter, NULL, NULL);
//playing
State state_wifi_play(&wifi_play_on_enter, NULL, NULL);
State state_wifi_stop(&wifi_stop_on_enter, NULL, NULL);

//FSMs
Fsm wifi_menu_fsm(&state_disp_KXLU);
Fsm wifi_play_fsm(&state_wifi_stop);

//entry functions for display
void disp_KXLU_on_enter()
{
    station_index = 0;
    display_station = String(stations[station_index].first.c_str());
    lcd.clear();
    lcd.drawString(0, 0, display_station.c_str());
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_LAist_on_enter()
{
    station_index = 1;
    display_station = String(stations[station_index].first.c_str());
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_KCRWLive_on_enter()
{
    station_index = 2;
    display_station = String(stations[station_index].first.c_str());
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_KCRWMusic_on_enter()
{
    station_index = 3;
    display_station = String(stations[station_index].first.c_str());
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_KRCC_on_enter()
{
    station_index = 4;
    display_station = String(stations[station_index].first.c_str());
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
void disp_Custom_on_enter()
{
    station_index = 5;
    display_station = String(stations[station_index].first.c_str());
    lcd.clear();
    lcd.print(display_station);
    if (display_station == playing_station){
        lcd.setCursor(0, 1);
        lcd.print("Playing");
    }
}
//play/stop function
void wifi_play_on_enter()
{
    //check string isn't empty
    if (stations[station_index].second.empty()){
        Serial.println("Station string is empty!");
        return;
    }
    //update playing station
    playing_station = display_station;
    lcd.setCursor(0, 1);
    lcd.print("Connecting");
    //try to connect with timeout
    bool connected = false;
    Serial.println("connecting to: ");
    Serial.print(stations[station_index].first.c_str());
    Serial.print(stations[station_index].second.c_str());
    connected = audio_ptr->connecttohost(stations[station_index].second.c_str());
    if (connected){
    lcd.setCursor(0,1);
    lcd.clearLine(1);
    lcd.print("Playing");
    Serial.println("connection made to: ");
    Serial.print(stations[station_index].first.c_str());
    Serial.print(stations[station_index].second.c_str());
    } else {
    //if fail, switch to mute state
    Serial.println("connection failed to: ");
    Serial.print(stations[station_index].first.c_str());
    Serial.print(stations[station_index].second.c_str());
    lcd.setCursor(0,1);
    lcd.print("Connection Failed!");
    delay(1000);
    wifi_play_fsm.trigger(ENTER_TRIG);
    }
}
  

void wifi_stop_on_enter()
{
    playing_station = "";
    lcd.setCursor(0, 1);
    lcd.clearLine(1);
}



void wifi_config(){
    Serial.println("building station list");
    build_station_list();
    Serial.println("built station list");
    lcd.clear();
    lcd.print("Internet Mode");
    //read network credentials
    SSID = NVS.getString(NVS_SSID);
    PSWD = NVS.getString(NVS_PSWD);
    Serial.println("read wifi credentials");
    //create the audio system
    audio_ptr = new Audio();
    Serial.println("made new audio");
    audio_ptr->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    Serial.println("pins set");
    audio_ptr->setVolume(10);
    Serial.println("volume set");
    Serial.println("audio init'd");
    //start connecting to wifi
    Serial.println("Connecting to WiFi");
    lcd.setCursor(0,1);
    lcd.print("Connecting to WiFi");
    WiFi.disconnect();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PSWD);
    int wifi_start_time = millis();
    //connection timeout management
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
    //internet connection validation
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
    lcd.clear();
    //add FSM transitions for display and boot menu
    wifi_menu_fsm.add_transition(&state_disp_KXLU, &state_disp_LAist, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_LAist, &state_disp_KCRWLive, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_KCRWLive, &state_disp_KCRWMusic, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_KCRWMusic, &state_disp_KRCC, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_KRCC, &state_disp_Custom, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_Custom, &state_disp_mode, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_mode, &state_disp_KXLU, SCROLL_TRIG, NULL);
    wifi_menu_fsm.add_transition(&state_disp_mode, &state_reboot, ENTER_TRIG, NULL);
    //add FSM transitions for audio mode
    wifi_play_fsm.add_transition(&state_wifi_stop, &state_wifi_play, ENTER_TRIG, NULL);
    wifi_play_fsm.add_transition(&state_wifi_play, &state_wifi_stop, ENTER_TRIG, NULL);
    //start FSM
    wifi_menu_fsm.run_machine();
    wifi_play_fsm.run_machine();
};

void wifi_loop(int trigger){
    switch (trigger)
    {
    case SCROLL_TRIG:
        wifi_menu_fsm.trigger(SCROLL_TRIG);
        wifi_play_fsm.trigger(SCROLL_TRIG);
        break;
    case ENTER_TRIG:
        wifi_menu_fsm.trigger(ENTER_TRIG);
        wifi_play_fsm.trigger(ENTER_TRIG);
        break;
    default:
        break;
    }
    if(!playing_station.isEmpty()){
        audio_ptr->loop();
    }
};

void build_station_list(){
    stations.push_back(make_pair("KXLU, 88.9",       "kxlu.streamguys1.com/kxlu-hi"));
    stations.push_back(make_pair("LAist, 89.3",      "http://3.88.129.151/southerncalipr-kpccfmmp3-imc?session-id=9a67f16aa32b96fef88ba726540a74cb&source=kpcc"));
    stations.push_back(make_pair("KCRW Live, 89.9",  "media.kcrw.com/pls/kcrwsimulcast.pls"));
    stations.push_back(make_pair("KCRW Music, 89.9", "media.kcrw.com/pls/kcrwmusic.pls"));
    stations.push_back(make_pair("KRCC, 91.5",       "streams.krcc.org/krcc_mp3"));
    //read custom station url and add to station vector
    custom_URL = NVS.getString(NVS_STAT).c_str();
    stations.push_back(make_pair("Custom", custom_URL.c_str()));
}