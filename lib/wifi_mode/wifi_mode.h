#include <radio_config.h>
#include <Audio.h>
#include <WiFi.h>
//declare station list
//custom station will be added in config function
std::vector<std::pair<const char*, const char*>> stations=
{
    {"KXLU, 88.9",       "kxlu.streamguys1.com/kxlu-hi"},
    {"LAist, 89.3",      "http://3.88.129.151/southerncalipr-kpccfmmp3-imc?session-id=9a67f16aa32b96fef88ba726540a74cb&source=kpcc"},
    {"KCRW Live, 89.9",  "media.kcrw.com/pls/kcrwsimulcast.pls"},
    {"KCRW Music, 89.9", "media.kcrw.com/pls/kcrwmusic.pls"},
    {"KRCC, 91.5",       "streams.krcc.org/krcc_mp3"}
};

void disp_KXLU_on_enter();
void disp_LAist_on_enter();
void disp_KCRWLive_on_enter();
void disp_KCRWMusic_on_enter();
void disp_KRCC_on_enter();
void disp_Custom_on_enter();

void wifi_config();
void wifi_loop();