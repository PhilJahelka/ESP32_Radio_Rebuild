#include <radio_config.h>

void disp_mode_on_enter()
{
    lcd.clear();
    lcd.print("Switch Mode?");
}
void reboot_on_enter()
{
    NVS.setString(NVS_MODE, String(MENU_MODE));
    Serial.println("Booting into menu mode");
    ESP.restart();
}

State state_disp_mode = State("disp_mode", disp_mode_on_enter);
State state_reboot = State("reboot_mode", reboot_on_enter);