#include <radio_config.h>
#include <Audio.h>
#include <WiFi.h>

//declare entry functions
void disp_KXLU_on_enter();
void disp_LAist_on_enter();
void disp_KCRWLive_on_enter();
void disp_KCRWMusic_on_enter();
void disp_KRCC_on_enter();
void disp_Custom_on_enter();

void wifi_play_on_enter();
void wifi_stop_on_enter();

void wifi_config();
void wifi_loop(int trigger);