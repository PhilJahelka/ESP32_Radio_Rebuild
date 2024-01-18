#include <wifi_mode.h>
#include <ESP32Ping.h>
#include <vector>
#include <WiFi.h>
#include <Audio.h>
#include <SPIFFS.h>
#include <fstream>
using namespace std;
/*
It is critical that the CSV defining the stations have \n termination, NOT \r\n
*/
//This has two FSMs, one for handling the display, another for play/pause state
//declare important variables
//pointer to eventually initialized audio system
Audio* audio_ptr;

int wifi_timeout = 10; //try connecting to WiFi for 10s
string custom_URL; //custom station URL from NVS
string playing_station; //currently playing station
string display_station; //station being displayed
int num_stations = 0; //total number of stations including SPIFFS and Custom
void build_station_list(); //create vector of stations 
vector<pair<string, string>> stations;
void build_display_FSM(); //create vector of states for display FSM
vector<State> display_states;

//what to do when entering a new display state
void disp_station_on_enter();

//handle play/pause FSM
void wifi_play_on_enter();
void wifi_stop_on_enter();

//States for play/pause FSM
State state_wifi_play("playing", wifi_play_on_enter, NULL, NULL);
State state_wifi_stop("stopped", wifi_stop_on_enter, NULL, NULL);

//FSM declarations
SimpleFSM wifi_play_fsm();
SimpleFSM wifi_menu_fsm();

//entry functions for display

void disp_station_on_enter()
{
    display_station = stations[station_index].first.c_str();
    lcd.clear();
    lcd.print(display_station.c_str());
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
    const char* URL = stations[station_index].second.c_str();
    lcd.setCursor(0, 1);
    lcd.print("Connecting");
    //try to connect with timeout
    bool connected = false;
    Serial.println("Attempting to connect to:");
    Serial.println(URL);
    Serial.println(playing_station.c_str());
    connected = audio_ptr->connecttohost(URL);
    if (connected){
    lcd.setCursor(0,1);
    lcd.clearLine(1);
    lcd.print("Playing");
    } else {
    //if fail, switch to mute state
    Serial.println("connection failed to: ");
    Serial.print(stations[station_index].first.c_str());
    Serial.print(URL);
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
    if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    lcd.clear();
    lcd.print("SPIFFS Broken!");
    delay(1000);
    force_menu_boot();
    }
    Serial.println("building station list");
    build_station_list();
    Serial.println("built station list");
    lcd.clear();
    lcd.print("Internet Mode");
    //read network credentials
    string SSID = string( NVS.getString(NVS_SSID).c_str() );
    string PSWD = string( NVS.getString(NVS_PSWD).c_str() );
    Serial.println("read wifi credentials");
    Serial.print("SSID is: ");
    Serial.println(SSID.c_str());
    Serial.print("PSWD is: ");
    Serial.println(PSWD.c_str());
    //create the audio system
    audio_ptr = new Audio();
    Serial.println("made new audio");
    audio_ptr->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    Serial.println("pins set");
    audio_ptr->setVolume(10);
    audio_ptr->setConnectionTimeout(1000, 2000);
    Serial.println("volume set");
    Serial.println("audio init'd");
    //start connecting to wifi
    Serial.println("Connecting to WiFi");
    lcd.setCursor(0,1);
    lcd.print("Connecting to WiFi");
    WiFi.disconnect();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID.c_str(), PSWD.c_str());
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
    build_display_FSM();
    //add FSM transitions for audio mode
    Transition play_transitions[] = {
        Transition(&state_wifi_stop, &state_wifi_play, ENTER_TRIG, NULL),
        Transition(&state_wifi_play, &state_wifi_stop, ENTER_TRIG, NULL)
    };
    wifi_play_fsm.add( play_transitions, 2);
    wifi_play_fsm.setInitialState(&state_wifi_stop)
    //start FSM
    wifi_menu_fsm.run();
    wifi_play_fsm.run();
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
    if(playing_station.empty() != true){
        audio_ptr->loop();
    }
};

void build_station_list(){
    //open file
    File station_file = SPIFFS.open("/stations.csv");
    //check file exists, if not boot to menu
    if(!station_file){
    Serial.println("Failed to open station file for reading");
    lcd.clear();
    lcd.print("No Station File");
    force_menu_boot();
    }
    //read each line of the file and add to stations_list
    while( station_file.available() ){
        string name = station_file.readStringUntil(',').c_str();
        string freq = station_file.readStringUntil(',').c_str();
        string URL  = station_file.readStringUntil('\n').c_str();
        stations.push_back(make_pair(name + ", " + freq, URL));
        Serial.println(stations.back().first.c_str());
        Serial.println(stations.back().second.c_str());
    }
    station_file.close();
    custom_URL = NVS.getString(NVS_STAT).c_str();
    stations.push_back(make_pair("Custom", custom_URL));
    num_stations = stations.size();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}

//want function that generates States from the stations vector and tacks on the disp_mode state
//use two different entry functions. One for if it's a radio station, and another for the mode

void build_display_FSM(){
    //generate a state of each station in the stations vector
    for( int i = 0; i < num_stations; i++)
    {
        display_states.push_back( State(stations[i].first.c_str(), disp_station_on_enter, NULL, NULL) );
    }
    //add the menu mode state
    display_states.push_back( state_disp_mode );
    //add transitions between playable stations
    for( int i = 0; i < num_stations - 1; i++)
    {
        wifi_menu_fsm.add
        (&display_states[i], &display_states[i+1], SCROLL_TRIG, NULL);
    }
    //add transition to menu mode
    wifi_menu_fsm_ptr->add_transition(&display_states.back(), &state_disp_mode, SCROLL_TRIG, NULL);
    //close the menu loop
    wifi_menu_fsm_ptr->add_transition(&state_disp_mode, &display_states.front(), SCROLL_TRIG, NULL);
    wifi_menu_fsm_ptr->run_machine();
}