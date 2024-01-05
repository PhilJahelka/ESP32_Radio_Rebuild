#include <menu_mode.h>
//define states

State state_disp_bt_mode(&disp_bt_mode_on_enter, NULL, NULL);
State state_disp_wifi_mode(&disp_wifi_mode_on_enter, NULL, NULL);
State state_disp_conf_mode(&disp_conf_mode_on_enter, NULL, NULL);
State state_boot_bt_mode(&bt_boot_on_enter, NULL, NULL);
State state_boot_wifi_mode(&wifi_boot_on_enter, NULL, NULL);
State state_boot_conf_mode(&conf_boot_on_enter, NULL, NULL);
Fsm menu_fsm(&state_disp_bt_mode);
//entry functions
void disp_bt_mode_on_enter()
{
    lcd.clear();
    lcd.print("Boot Into:");
    lcd.setCursor(0,1);
    lcd.print("BT Mode?");
}
void disp_wifi_mode_on_enter()
{
    lcd.clear();
    lcd.print("Boot Into:");
    lcd.setCursor(0,1);
    lcd.print("WiFi Mode?");
}
void disp_conf_mode_on_enter()
{
    lcd.clear();
    lcd.print("Boot Into:");
    lcd.setCursor(0,1);
    lcd.print("WiFi Conf Mode?");
}
void bt_boot_on_enter()
{
    NVS.setString(NVS_MODE, String(A2DP_MODE));
    Serial.println("Booting into BT mode");
    ESP.restart();
}
void wifi_boot_on_enter()
{
    NVS.setString(NVS_MODE, String(WIFI_MODE));
    Serial.println("Booting into WiFi mode");
    ESP.restart();
}
void conf_boot_on_enter()
{
    NVS.setString(NVS_MODE, String(CONF_MODE));
    Serial.println("Booting into WiFi Config mode");
    ESP.restart();
}

//bluetooth->WiFi->Conf->BT
void menu_config(){
    Serial.println("in menu mode");
    //define menu loop
    menu_fsm.add_transition(&state_disp_bt_mode, &state_disp_wifi_mode, SCROLL_TRIG, NULL);
    menu_fsm.add_transition(&state_disp_wifi_mode, &state_disp_conf_mode, SCROLL_TRIG, NULL);
    menu_fsm.add_transition(&state_disp_conf_mode, &state_disp_bt_mode, SCROLL_TRIG, NULL);
    //define mode set and boot
    menu_fsm.add_transition(&state_disp_bt_mode, &state_boot_bt_mode, ENTER_TRIG, NULL);
    menu_fsm.add_transition(&state_disp_wifi_mode, &state_boot_wifi_mode, ENTER_TRIG, NULL);
    menu_fsm.add_transition(&state_disp_conf_mode, &state_boot_conf_mode, ENTER_TRIG, NULL);
    menu_fsm.run_machine();
    Serial.println("setup menu_fsm");
}

void menu_loop(int trigger){
    switch (trigger)
    {
    case SCROLL_TRIG:
        menu_fsm.trigger(SCROLL_TRIG);
        break;
    case ENTER_TRIG:
        menu_fsm.trigger(ENTER_TRIG);
        break;
    default:
        break;
    }
}