#include <Usb.h>
#include <usbhub.h>
#include <hidboot.h>
#include "Keyboard.h"

// キーの同時押しのときに複数回入力させないためのキャッシュ
bool keyCache[256];

boolean isKeyCached(uint8_t element) {
  return keyCache[element];
}

void initCacheArray() {
  for (int i = 0; i < 256; i++) {
    keyCache[i] = false;
  }
}

// キー送信のメイン処理
void sendKeyCodesBySerial(uint8_t modifiers,
                          uint8_t keycode0,
                          uint8_t keycode1,
                          uint8_t keycode2,
                          uint8_t keycode3,
                          uint8_t keycode4,
                          uint8_t keycode5)
{
  //  Keyboard.print(0xFD); // Raw Report Mode
  //  Keyboard.print(0x09); // Length
  //  Keyboard.print(0x01); // Descriptor 0x01=Keyboard
  //
  //  /* send key codes(8 bytes all) */
  //Keyboard.print(modifiers); // modifier keys
  //  Keyboard.print(0x00, 1);   // reserved
  //Keyboard.print(keycode0);  // keycode0
  // デバッグ出力用
  Serial.print("modifiers:"); // modifier keys
  Serial.println(modifiers); // modifier keys
  Serial.print("keycode0:");  // keycode
  Serial.println(keycode0);  // keycode
  Serial.print("keycode1:");  // keycode
  Serial.println(keycode1);  // keycode
  Serial.print("keycode2:");  // keycode
  Serial.println(keycode2);  // keycode
  Serial.println("");  // keycode

  // すべてのキーが離されたらキャッシュクリア
  if (keycode0 == 0 && keycode1 == 0 && keycode2 == 0 && keycode3 == 0 && keycode4 == 0 && keycode5 == 0) {
    initCacheArray();
    Keyboard.releaseAll();
    delay(5);
    return;
  }

  // キャッシュがあったら入力をさせない
  if (isKeyCached(keycode0)) keycode0 = 0;
  if (isKeyCached(keycode1)) keycode1 = 0;
  if (isKeyCached(keycode2)) keycode2 = 0;
  if (isKeyCached(keycode3)) keycode3 = 0;
  if (isKeyCached(keycode4)) keycode4 = 0;
  if (isKeyCached(keycode5)) keycode5 = 0;

  // 入力されたキー情報はキャッシュしておく
  keyCache[keycode0] = true;
  keyCache[keycode1] = true;
  keyCache[keycode2] = true;
  keyCache[keycode3] = true;
  keyCache[keycode4] = true;
  keyCache[keycode5] = true;

  // 修飾キーの入力
  pressModifiers(modifiers);
  // 実際のキー入力
  mapKeycodeToString(modifiers, keycode0);
  mapKeycodeToString(modifiers, keycode1);
  mapKeycodeToString(modifiers, keycode2);
  mapKeycodeToString(modifiers, keycode3);
  mapKeycodeToString(modifiers, keycode4);
  mapKeycodeToString(modifiers, keycode5);
  Keyboard.releaseAll();
  delay(5);
}

void pressModifiers(uint8_t modifiers) {
  if (modifiers == 1) {
    Keyboard.press(KEY_LEFT_CTRL);
  }
  else if (modifiers == 2) {
    Keyboard.press(KEY_LEFT_SHIFT);
  }
}

void mapKeycodeToString(uint8_t modifiers, uint8_t keycode) {


  char mapArray[] = {
    'NA', 'NA', 'NA', 'NA', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'NA', 'NA', 'NA', 'NA', ' ', '-', '=', '[', ']', 'NA',
    'NA', ';', '\'', 'NA', ',', '.', '/', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', '`', 'NA', '\\', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA',
    'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA', 'NA'
  };

  if (keycode == 79) {
    Keyboard.press(KEY_RIGHT_ARROW);
  }
  else if (keycode == 80) {
    Keyboard.press(KEY_LEFT_ARROW);
  }
  else if (keycode == 81) {
    Keyboard.press(KEY_DOWN_ARROW);
  }
  else if (keycode == 82) {
    Keyboard.press(KEY_UP_ARROW);
  }
  else if (keycode == 42) {
    Keyboard.press(KEY_BACKSPACE);
  }
  else if (keycode == 40) {
    Keyboard.press(KEY_RETURN);
  }
  else if (keycode == 138) {
    Keyboard.press(KEY_LEFT_ALT);
  }
  else if (keycode == 136) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('`');
  }
  else if (keycode == 0) {
  }
  else {
    Keyboard.press(mapArray[keycode]);
  }

  return;
}

class HIDKeyboardParser : public KeyboardReportParser
{
    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void HIDKeyboardParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  sendKeyCodesBySerial(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
}

//boolean arrayContainsElement(uint8_t array[], uint8_t element) {
//  for (int i = 0; i < 6; i++) {
//    if (array[i] == element) {
//      return true;
//    }
//  }
//  return false;
//}


USB usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&usb);
uint32_t next_time;
HIDKeyboardParser parser;

void setup()
{
  Keyboard.begin();

  usb.Init();
  delay(200);
  next_time = millis() + 5000;

  HidKeyboard.SetReportParser(0, (HIDKeyboardParser*)&parser);
}

void loop()
{
  usb.Task();
}


