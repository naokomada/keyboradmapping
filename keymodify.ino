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

void outputCacheArray() {
  for (int i = 0; i < 256; i++) {
    Serial.print(i, DEC);
    Serial.println(keyCache[i] ? " true" : " false");
  }
}

// キー送信のメイン処理
void sendKeyCodesBySerial(uint8_t modifiers,
                          uint8_t *keycodes,
                          bool isKeyRepeat)
{
  //  Keyboard.print(0xFD); // Raw Report Mode
  //  Keyboard.print(0x09); // Length
  //  Keyboard.print(0x01); // Descriptor 0x01=Keyboard

  // デバッグ出力用
  //outputCacheArray();
  Serial.print("modifiers:"); // modifier keys
  Serial.println(modifiers); // modifier keys
  for (int i = 0; i < 6; i++) {
    Serial.print("keycode:");
    Serial.println(keycodes[i]);
  }
  Serial.print("isKeyRepeat:");
  Serial.println(isKeyRepeat);
  Serial.println("");  // keycode


  // 修飾キー以外すべてのキーが離されたらキャッシュクリア

  if ((keycodes[0] == 0 || keycodes[0] == 138 || keycodes[0] == 139)
      && (keycodes[1] == 0 || keycodes[1] == 138 || keycodes[1] == 139)
      && (keycodes[2] == 0 || keycodes[2] == 138 || keycodes[2] == 139)
      && (keycodes[3] == 0 || keycodes[3] == 138 || keycodes[3] == 139)
      && (keycodes[4] == 0 || keycodes[4] == 138 || keycodes[4] == 139)
      && (keycodes[5] == 0 || keycodes[5] == 138 || keycodes[5] == 139)) {
    initCacheArray();
    Keyboard.releaseAll();
    //delay(5);
    return;
  }


  // 変換キーをSHIFTにする
  if (keycodes[0] == 138 || keycodes[1] == 138 || keycodes[2] == 138 || keycodes[3] == 138 || keycodes[4] == 138 || keycodes[5] == 138) {
    modifiers = 2;
  }
  // 無変換キーをCtrlにする
  if (keycodes[0] == 139 || keycodes[1] == 139 || keycodes[2] == 139 || keycodes[3] == 139 || keycodes[4] == 139 || keycodes[5] == 139) {
    modifiers = 1;
  }

  // 修飾キーの入力
  pressModifiers(modifiers);

  // 実際のキー入力
  for (int i = 0; i < 6; i++) {
    // 修飾キー扱いのキーはキャッシュ対象外にする
    if (keycodes[i] == 138 || keycodes[i] == 139) continue;

    // キャッシュがあったら入力をさせない
    if (isKeyCached(keycodes[i])) keycodes[i] = 0;

    // 入力されたキー情報はキャッシュしておく
    keyCache[keycodes[i]] = true;
    pressNormalKey(keycodes[i]);
  }

  Keyboard.releaseAll();
  //delay(5);
}

void pressModifiers(uint8_t modifiers) {
  if (modifiers == 1) {
    Keyboard.press(KEY_LEFT_CTRL);
  }
  else if (modifiers == 2) {
    Keyboard.press(KEY_LEFT_SHIFT);
  }
  else if (modifiers == 4) {
    Keyboard.press(KEY_LEFT_ALT);
  }
  else if (modifiers == 8) {
    Keyboard.press(KEY_LEFT_GUI);
  }
}

void pressNormalKey(uint8_t keycode) {

  char charMapArray[] = {
    'N', 'N', 'N', 'N', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    KEY_RETURN, KEY_ESC, KEY_BACKSPACE, KEY_TAB, ' ', '-', '=', '[', ']', 'N',
    'N', ';', '\'', 'N', ',', '.', '/', 'N', KEY_F1, KEY_F2,
    KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    'N', 'N', 'N', KEY_INSERT, KEY_HOME, KEY_PAGE_UP, KEY_DELETE, KEY_END, KEY_PAGE_DOWN, KEY_RIGHT_ARROW,
    KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, 'N', 'N', 'N', 'N', 'N', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', '`', 'N', '\\', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N',
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'
  };

  if (keycode == 136) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('`');
  }
  // 138は変換キーをSHIFTにしたため
  else if (keycode == 0 || keycode == 138 || keycode == 139) {
  }
  else {
    Keyboard.press(charMapArray[keycode]);
  }

  return;
}

class HIDKeyboardParser : public KeyboardReportParser
{
    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void HIDKeyboardParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  uint8_t modifier = buf[0];
  uint8_t keycodes[6] = {buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]};
  sendKeyCodesBySerial(modifier, keycodes, is_rpt_id);
  Serial.print("isKeyRepeat:");
  Serial.println(is_rpt_id);
}

USB usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&usb);
HIDKeyboardParser parser;

void setup()
{
  Keyboard.begin();

  usb.Init();
  delay(200);

  HidKeyboard.SetReportParser(0, (HIDKeyboardParser*)&parser);
}

void loop()
{
  usb.Task();
}


