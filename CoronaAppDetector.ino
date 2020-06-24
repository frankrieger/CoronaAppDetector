// Simple rough fork of the CoronaAppDetector adapted for M5StickC
// original initial source from https://github.com/kauzu/CoronaAppDetector
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <Ticker.h>
#include <unordered_set>
#include <M5StickC.h>


#define SLOTS 3
#define TICK_INTERVAL 10

String Address = "00:AA:FF:13:37:42";
 
Ticker Tic;
static BLEAddress *pServerAddress;
BLEScan* pBLEScan ;
int scanTime = 10; 


int slot = 0;
int old = 0;
std::unordered_set<std::string>* seen[SLOTS];
void sTick()  // Wird jede Sekunde ausgefüert
{ 
  slot = ++slot%SLOTS; //increment in %10
  seen[slot]->clear();//clear
  int sum = 0;
  int fullSlots = 0;
  /*the problem i'm solving with that is that after 15 minutes a device changes its mac, so we can only track a device for a 15 minute period, but it we take the 
  average number of devices seen in the last 100 seconds and round down we should be fine. also i didn't want to implement some median function which would have been better*/ 
  for (int i = 0; i < SLOTS; i++)
  {
    std::unordered_set<std::string>::iterator it = seen[i]->begin();

    if (seen[i]->size() > 0)
        Serial.println("============");
    // Iterate till the end of set
    while (it != seen[i]->end())
    {
      Serial.println(it->c_str());
      it++;
    }
    sum = sum+ seen[i]->size();
    if (seen[i]->size() > 0)
      fullSlots++;
  }
  int near = 0;
  if (fullSlots > 0)
    near = sum / fullSlots;
  
  //tell serial the number of devices
  if (near != old){
    Serial.println("Number of devices:");
    Serial.println(near);
    old = near;
  }
  // print to 7segment display. 
  // printNumber(near);
   M5.Lcd.fillScreen(BLACK);
  delay(500);
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(1, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(6);
  M5.Lcd.print(near);
}
 
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) //is called when a ble beacon is found
    {
      BLEUUID coronaUUID("FD6F"); //from https://covid19-static.cdn-apple.com/applications/covid19/current/static/contact-tracing/pdf/ExposureNotification-BluetoothSpecificationv1.2.pdf
      if (advertisedDevice.isAdvertisingService(coronaUUID)) //if this is a corona beacon
      {
        seen[slot]->insert(advertisedDevice.getAddress().toString()); //save sender mac to the set
      }
      else if (advertisedDevice.haveServiceUUID())
      {
        Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
        
      }
      else
      {
        Serial.println(advertisedDevice.toString().c_str());
      }
    }
};


 
void setup() 
{
  M5.begin();
 M5.Lcd.fillScreen(BLUE);
  delay(500);
  M5.Lcd.fillScreen(BLACK);
  delay(500);
M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Covid App Counter");
  
  //we have 10 slots, each stores the macs seen in a second. so we record the seen macs from the last 10 secods
  for (int i = 0; i < SLOTS; i++)
  {
    seen[i] = new std::unordered_set<std::string>();
  }
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starte BLE Scanner");
  pServerAddress = new BLEAddress(Address.c_str());
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
 

  //every TICK_INTERVAL second we want to do some stuff
  Tic.attach( TICK_INTERVAL,sTick);
}
 
void loop()
{
  pBLEScan->start(scanTime);
}
