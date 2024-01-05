#include <conf_mode.h>

//declare server
NimBLEServer *pServer = NULL;
bool deviceConnected = false;
//declare variable for things received over BT
String rxValue;

//UUIDs for setting up the service
const static char* SERVICE_UUID                = "d1f00001-f666-4350-905a-761de84178c1";
const static char* CHARACTERISTIC_UUID_SSID    = "d1f00002-f666-4350-905a-761de84178c1";
const static char* CHARACTERISTIC_UUID_PSWD    = "d1f00003-f666-4350-905a-761de84178c1";
const static char* CHARACTERISTIC_UUID_STAT    = "d1f00004-f666-4350-905a-761de84178c1";
const static NimBLEUUID DESCRIPTOR_UUID_NAME   = NimBLEUUID((uint16_t)0x2901); //UUID for human readable characteristic descriptors


//Callbacks for device dis/connection
class MyServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
      deviceConnected = true;
      Serial.println("device connected");
    };

    void onDisconnect(NimBLEServer* pServer) {
      deviceConnected = false;
    }
};

//Callbacks for writing data to characteristics
class SSIDCallback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic) {
      rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.print("New SSID: ");
        Serial.println(rxValue);
        NVS.setString("SSID", rxValue);
      }
    }
};

class PSWDCallback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic) {
      rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.print("New Password: ");
        Serial.println(rxValue);
        NVS.setString("PSWD", rxValue);
      }
    }
};

class STATCallback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic) {
      rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.print("New Custom Station: ");
        Serial.println(rxValue);
        NVS.setString("STAT", rxValue);
      }
    }
};


//entry functions
void disp_conf_on_enter()
{
    lcd.clear();
    lcd.print("Conf BT Name:");
    lcd.setCursor(0,1);
    lcd.print(BT_NAME);
}
//setup the finite state machine. Basically same as A2DP_mode
//define states
State state_disp_conf(&disp_conf_on_enter, NULL, NULL);
//init machine
Fsm conf_fsm(&state_disp_conf);



void conf_config() {
    Serial.println("In config setup");
    // Create the NimBLE Device
    NimBLEDevice::init(BT_NAME);
    // Create the NimBLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    // Create the NimBLE Service
    NimBLEService *pService = pServer->createService(SERVICE_UUID);
    // Create characteristics
    NimBLECharacteristic * pSSIDCharacteristic = pService->createCharacteristic(
											CHARACTERISTIC_UUID_SSID,
											NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
										);
    pSSIDCharacteristic->setCallbacks(new SSIDCallback());
    pSSIDCharacteristic->setValue( NVS.getString("SSID") );
  
    NimBLECharacteristic * pPSWDCharacteristic = pService->createCharacteristic(
											CHARACTERISTIC_UUID_PSWD,
											NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
										);
    pPSWDCharacteristic->setCallbacks(new PSWDCallback());
    pPSWDCharacteristic->setValue( NVS.getString("PSWD") );

    NimBLECharacteristic * pSTATCharacteristic = pService->createCharacteristic(
											CHARACTERISTIC_UUID_STAT,
											NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
										);
    pSTATCharacteristic->setCallbacks(new STATCallback());
    pSTATCharacteristic->setValue( NVS.getString("STAT") );

    //Create Descriptors to label characteristics and credentials and their desired formats as UTF-8
    NimBLEDescriptor* pSSIDDescriptor_name = pSSIDCharacteristic->createDescriptor(DESCRIPTOR_UUID_NAME, NIMBLE_PROPERTY::READ);
    pSSIDDescriptor_name->setValue("Network SSID");

    NimBLEDescriptor* pPSWDDescriptor_name = pPSWDCharacteristic->createDescriptor(DESCRIPTOR_UUID_NAME, NIMBLE_PROPERTY::READ);
    pPSWDDescriptor_name->setValue("Network Password");

    NimBLEDescriptor* pSTATDescriptor_name = pSTATCharacteristic->createDescriptor(DESCRIPTOR_UUID_NAME, NIMBLE_PROPERTY::READ);
    pSTATDescriptor_name->setValue("Custom URL");

    NimBLE2904* pSSID2904 = (NimBLE2904*)pSSIDCharacteristic->createDescriptor("2904");
    pSSID2904->setFormat(NimBLE2904::FORMAT_UTF8);
    NimBLE2904* pPSWD2904 = (NimBLE2904*)pPSWDCharacteristic->createDescriptor("2904");
    pPSWD2904->setFormat(NimBLE2904::FORMAT_UTF8);
    NimBLE2904* pSTAT2904 = (NimBLE2904*)pSTATCharacteristic->createDescriptor("2904");
    pSTAT2904->setFormat(NimBLE2904::FORMAT_UTF8);
  
    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");

    //define transitions
    conf_fsm.add_transition(&state_disp_conf, &state_disp_mode, SCROLL_TRIG, NULL);
    conf_fsm.add_transition(&state_disp_mode, &state_disp_conf, SCROLL_TRIG, NULL);
    conf_fsm.add_transition(&state_disp_mode, &state_reboot, ENTER_TRIG, NULL);
    conf_fsm.run_machine();
}

void conf_loop(int trigger) {
    switch (trigger)
    {
    case SCROLL_TRIG:
        conf_fsm.trigger(SCROLL_TRIG);
        break;
    case ENTER_TRIG:
        conf_fsm.trigger(ENTER_TRIG);
        break;
    default:
        break;
    }
    // disconnecting
    if (!deviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
    }
}