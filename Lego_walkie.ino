/*********************************************************************
 * This code is for the project at 
 * http://www.iliketomakestuff.com/how-to-make-a-lego-walkie-talkie-twitch-controller
 * All of the components are list on the url above.
 * 
 * This script was created by Bob Clagett for I Like To Make Stuff
 * For more projects, check out iliketomakestuff.com
 * Includes Modified version of the "hid_keyboard" example for the Adafruit nRF52 based Bluefruit LE modules
 * 
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 * 
 * MIT license, check LICENSE for more information
 * All text above, and the splash screen below must be included in
 * any redistribution
*********************************************************************/
#include <bluefruit.h>

BLEDis bledis;
BLEHidAdafruit blehid;
int bt1Pin = A0;
int bt2Pin = A2;
int bt3Pin = A4;
char lastPressed = '@';
char pressed = '@';

//set keys to send here
char bt1Char = '1';
char bt2Char = '2';
char bt3Char = '3';

bool hasKeyPressed = false;

void setup() 
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  pinMode(bt1Pin, INPUT_PULLUP);
  pinMode(bt2Pin, INPUT_PULLUP);
  pinMode(bt3Pin, INPUT_PULLUP);

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("LegoWalkieTalkie");

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
  blehid.begin();

  // Set callback for set LED from central
  //blehid.setKeyboardLedCallback(set_keyboard_led);

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
  /* Bluefruit.setConnInterval(9, 12); */

  // Set up and start advertising
  startAdv();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop() 
{
  // Only send KeyRelease if previously pressed to avoid sending
  // multiple keyRelease reports (that consume memory and bandwidth)
  if(digitalRead(bt1Pin)==LOW){
      pressed = bt1Char;
      hasKeyPressed=true;
  } else if(digitalRead(bt2Pin)==LOW){
      pressed = bt2Char;
      hasKeyPressed=true;
  } else if(digitalRead(bt3Pin)==LOW){
      pressed = bt3Char;
      hasKeyPressed=true;
  }
    
  if ( hasKeyPressed )
  {
    hasKeyPressed = false;
    blehid.keyRelease();
    
    // Delay a bit after a report
    delay(5);
  if(lastPressed != pressed){
    // echo
    Serial.write(pressed);
    blehid.keyPress(pressed);
    lastPressed = pressed;
  }
    // Delay a bit after a report
    delay(5);
  }

  // Request CPU to enter low-power mode until an event/interrupt occurs
  waitForEvent();  
}
