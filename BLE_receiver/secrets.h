/***** Wi-Fi Credentials *****/
//#define STA_SSID "iPhone Isabel"
//#define STA_PASS "hotspotisabel"
//#define STA_SSID "eot.pt"
//#define STA_PASS "eot_2016"
//#define STA_SSID "NOS-9D70"
//#define STA_PASS "82cdeb96f5db"
#define STA_SSID "LAPTOP-ISABEL"
#define STA_PASS "HotspotIsabel"
//#define STA_SSID "Vodafone-342B0A"
//#define STA_PASS "dxgt2dqxUN"


/***** The ThingsBoard parameters *****/

// if cloud/professional edition 
#define THINGSBOARD_SERVER  "iot.istartlab.tecnico.ulisboa.pt"

// if community edition live demo
// #define THINGSBOARD_SERVER  "demo.thingsboard.io"

//BLE Reveiver Device Token
//#define TOKEN "xBKIxRmBAR9QzLdDCXGz" //esp1
//#define TOKEN "LiAv1GOIKo2QW2muqONS" //esp2 
#define TOKEN "jaJv4liL5EJsUDLj9MWt" //esp3


/***** BLE System UUID *****/
// same for all beacons and all BLE Receivers
uint8_t uuid_system[16] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };