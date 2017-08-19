// C64 USB Keyboard mod
// Original by Pyofer
// See original thread @
// http://www.lemon64.com/forum/viewtopic.php?t=55650
//
// Modified to Support restore key & US/EU keymaps by dabone.
// Wiring is as follows
// 64 Keyboard connector has 20 Pins with a key Pin @ Pin 2.
// Arduino Pro Micro Pinout
// https://cdn.sparkfun.com/assets/9/c/3/c/4/523a1765757b7f5c6e8b4567.png

// Board type should be Arduino Leonardo (or the Pro mini clones)


//KeyBoard     Arduino Pro Micro
//   20           2 - SDA
//   19           3 - SCL
//   18           4 - A6
//   17           5 -
//   16           6 - A7
//   15           7 -
//   14           8 - A8
//   13           9 - A9
//   12           10 - A10
//   11           16 - MOSI
//   10           14 - MISO
//   9            15 - SCLK
//   8            18 - A0
//   7            19 - A1
//   6            20 - A2
//   5            21 - A3
//   4            N/C
//   3            1 - TXD
//   2            N/C
//   1            8 - A8


// Keyboard Matrix Now Matches real C64 with one more column.
// Matrix can be found at
// http://sta.c64.org/cbm64kbdlay.html

// For Standard Keys See
// http://www.asciitable.com/
//
// Arduino Keyboard Modifier Keys
// Key                  Decimal value
// KEY_LEFT_CTRL            128
// KEY_LEFT_SHIFT           129
// KEY_LEFT_ALT             130
// KEY_LEFT_GUI             131
// KEY_RIGHT_CTRL           132
// KEY_RIGHT_SHIFT          133
// KEY_RIGHT_ALT            134
// KEY_RIGHT_GUI            135
// KEY_UP_ARROW             218
// KEY_DOWN_ARROW           217
// KEY_LEFT_ARROW           216
// KEY_RIGHT_ARROW          215
// KEY_BACKSPACE            178
// KEY_TAB                  179
// KEY_RETURN               176
// KEY_ESC                  177
// KEY_INSERT               209
// KEY_DELETE               212
// KEY_PAGE_UP              211
// KEY_PAGE_DOWN            214
// KEY_HOME                 210
// KEY_END                  213
// KEY_CAPS_LOCK            193
// KEY_F1                   194
// KEY_F2                   195
// KEY_F3                   196
// KEY_F4                   197
// KEY_F5                   198
// KEY_F6                   199
// KEY_F7                   200
// KEY_F8                   201
// KEY_F9                   202
// KEY_F10                  203
// KEY_F11                  204
// KEY_F12                  205
// Keypad /                 220
// Keypad *                 221
// Keypad -                 222
// Keypad +                 223
// Keypad ENTER             224
// Keypad 1 & End           225
// Keypad 2 & Down Arrow    226
// Keypad 3 & PageDn        227
// Keypad 4 & Left Arrow    228
// Keypad 5                 229
// Keypad 6 & Right Arrow   230
// Keypad 7 & Home          231
// Keypad 8 & Up Arrow      232
// Keypad 9 & PageUp        233
// Keypad 0 & Insert        234
// Keypad . & Delete        235

#include <HID.h>
#include <Keyboard.h>

int inChar = 0;
int keyPos = 0;
int digitalread = 0;
int keyDown[72];
long lastDebounceTime[72];
unsigned long time;
int debounceDelay = 50;
int shift = 0;
int outPin = 2;
int outPinSet = 0;
int i;
int windowsShift;
int DefaultKBMode = 1;                             // Select 0 For Windows Mode On startup or 1 for C64 Mode
int USKeyboard = 1;                                // Select 1 for US Keyboard or 0 For EU

char keyMapUS[216] = {

  212,176,215,200,194,196,198,217,0,                //  Del Return LR F7 F1 F3 F5 UD Null
  51,119,97,52,122,115,101,129,0,                   //  3 W A 4 Z S E LSHFT Null
  53,114,100,54,99,102,116,120,0,                   //  5 R D 6 C F T X Null
  55,121,103,56,98,104,117,118,0,                   //  7 Y G 8 B H U V Null
  57,105,106,48,109,107,111,110,0,                  //  9 I J Zero M K O N Null
  43,112,108,45,46,58,64,44,0,                      //  + P L - . : @ , Null     (US Keyboard)
  35,42,59,210,133,61,211,47,205,                   //  Pound * ; Home RSHFT = Pi / Restore
  49,178,128,50,32,179,113,177,0,                   //  1 BS CTRL 2 SPC C= Q RunStop Null

  209,176,216,201,195,197,199,218,0,                //  Del Return LR F8 F2 F4 F6 UD Null
  92,87,65,36,90,83,69,129,0,                       //  # W A $ Z S E LSHFT Null
  37,82,68,38,67,70,84,88,0,                        //  % R D & C F T X Null
  39,89,71,40,66,72,85,86,0,                        //  ' Y G ( B H U V Null
  41,73,74,48,77,75,79,78,0,                        //  ) I J Zero M K O N Null
  43,80,76,95,62,91,96,60,0,                        //  + P L - > : @ < Null
  35,42,93,210,133,61,214,63,205,                   //  Pound * ; Home RSHFT = Pi ? Restore
  33,178,128,34,32,179,81,177,0,                    //  ! BS CTRL " SPC C= Q RS Null     (US Keyboard)

  178,176,215,200,194,196,198,217,0,                //  Del Return LR F7 F1 F3 F5 UD Null
  51,119,97,52,122,115,101,129,0,                   //  3 W A 4 Z S E LSHFT Null
  53,114,100,54,99,102,116,120,0,                   //  5 R D 6 C F T X Null
  55,121,103,56,98,104,117,118,0,                   //  7 Y G 8 B H U V Null
  57,105,106,48,109,107,111,110,0,                  //  9 I J Zero M K O N Null
  45,112,108,61,46,59,91,44,0,                      //  + P L - . : @ , Null
  209,93,39,210,133,92,212,47,205,                  //  Pound * ; Home RSHFT = Pi / Restore
  49,223,9,50,32,128,113,177,0,                     //  1 BS CTRL 2 SPC C= Q RunStop Null

};

char keyMapEU[216]={
  212,176,215,200,194,196,198,217,0,                //  Del Return LR F7 F1 F3 F5 UD Null
  51,119,97,52,122,115,101,129,0,                   //  3 W A 4 Z S E LSHFT Null
  53,114,100,54,99,102,116,120,0,                   //  5 R D 6 C F T X Null
  55,121,103,56,98,104,117,118,0,                   //  7 Y G 8 B H U V Null
  57,105,106,48,109,107,111,110,0,                  //  9 I J Zero M K O N Null
  43,112,108,45,46,58,34,44,0,                      //  + P L - . : @ , Null     (EU Keyboard)
  35,42,59,210,133,61,211,47,205,                   //  Pound * ; Home RSHFT = Pi / Restore
  49,178,128,50,32,179,113,177,0,                   //  1 BS CTRL 2 SPC C= Q RunStop Null

  209,176,216,201,195,197,199,218,0,                //  Del Return LR F8 F2 F4 F6 UD Null
  92,87,65,36,90,83,69,129,0,                       //  # W A $ Z S E LSHFT Null
  37,82,68,38,67,70,84,88,0,                        //  % R D & C F T X Null
  39,89,71,40,66,72,85,86,0,                        //  ' Y G ( B H U V Null
  41,73,74,48,77,75,79,78,0,                        //  ) I J Zero M K O N Null
  43,80,76,95,62,91,96,60,0,                        //  + P L - > : @ < Null
  35,42,93,210,133,61,214,63,205,                   //  Pound * ; Home RSHFT = Pi ? Restore
  33,178,128,64,32,179,81,177,0,                    //  ! BS CTRL " SPC C= Q RS Null     (EU Keyboard)

  178,176,215,200,194,196,198,217,0,                //  Del Return LR F7 F1 F3 F5 UD Null
  51,119,97,52,122,115,101,129,0,                   //  3 W A 4 Z S E LSHFT Null
  53,114,100,54,99,102,116,120,0,                   //  5 R D 6 C F T X Null
  55,121,103,56,98,104,117,118,0,                   //  7 Y G 8 B H U V Null
  57,105,106,48,109,107,111,110,0,                  //  9 I J Zero M K O N Null
  45,112,108,61,46,59,91,44,0,                      //  + P L - . : @ , Null
  209,93,39,210,133,92,212,47,205,                  //  Pound * ; Home RSHFT = Pi / Restore
  49,223,9,50,32,128,113,177,0,                     //  1 BS CTRL 2 SPC C= Q RunStop Null

};

void setup() {

  // initialize control over the keyboard:
  Keyboard.begin();

  // Set all keys as up
  for (i = 0; i < 64; i++)
    keyDown[i] = 0;

  // configure inputs and outputs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  // use internal pullups to hold pins high
  pinMode(10, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  // start with one active pin to detect '1'
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);

  if (DefaultKBMode == 1) {
    // detect if '1' is held on power up to swap mode
    if (digitalRead(10))
      windowsShift = 0;
    else
      windowsShift = 1;
  }
  if (DefaultKBMode == 0) {
    // detect if '1' is held on power up to swap mod
    if (digitalRead(10))
      windowsShift = 1;
    else
      windowsShift = 0;
  }
}

// main keyboard scanning loop
void loop() {
  // scan through all rows
  for (outPin = 2; outPin < 10; outPin++) {
    // set unused (all) outputs to input to avoid ghosting
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    pinMode(8, INPUT);
    pinMode(9, INPUT);

    // Changed order to match real c64 keyboard matrix layout.
    if (outPin == 2)
      outPinSet = 9;
    else if (outPin == 9)
      outPinSet = 2;
    else
      outPinSet = outPin;
    pinMode(outPinSet, OUTPUT);
    digitalWrite(outPinSet, LOW);

    // scan through columns
    for (i = 0; i < 9; i++) {
      // calculate character map position
      keyPos = i + ((outPin - 2) * 9);
      if (USKeyboard == 1) {
        // work out which key it is from the map and shift if needed
        // else "windows" keymap where shift is passed through
        if (windowsShift)
          inChar = keyMapUS[keyPos + 144];
        else
          inChar = keyMapUS[keyPos + shift];
      } else {
        // work out which key it is from the map and shift if needed
        // else use "windows" keymap where shift is passed through
        if (windowsShift)
          inChar = keyMapEU[keyPos + 144];
        else
          inChar = keyMapEU[keyPos + shift];
      }
      // check the active input pin
      if (i == 0)
        digitalread = 1 - digitalRead(10);
      else if (i == 1)
        digitalread = 1 - digitalRead(16);
      else if (i == 2)
        digitalread = 1 - digitalRead(14);
      else if (i == 3)
        digitalread = 1 - digitalRead(A3);
      else if (i == 4)
        digitalread = 1 - digitalRead(A0);
      else if (i == 5)
        digitalread = 1 - digitalRead(A1);
      else if (i == 6)
        digitalread = 1 - digitalRead(A2);
      else if (i == 7)
        digitalread = 1 - digitalRead(15);
      else if (i == 8)
        digitalread = 1 - digitalRead(1);

      time = millis();
      // debounce for each key individually
      if ((time - lastDebounceTime[keyPos]) > debounceDelay) {
        // if a key is pressed and wasn't already down
        if (digitalread == 1 && keyDown[keyPos] == 0) {
          // put the right character in the keydown array
          keyDown[keyPos] = inChar;
          // is it not-shift or in windows mode?
          if ((keyPos != 16 && keyPos != 58) || windowsShift == 1) {
            // if so pass the key through
            // reset the debounce delay
            lastDebounceTime[keyPos] = time;
            // pass the keypress to windows
            Keyboard.press(keyDown[keyPos]);
          } else {
            // reset keybounce delay and mark as shift press
            lastDebounceTime[keyPos] = time;
            shift = 72;
          }
        }
        // key is up and a character is stored in the keydown position
        if (digitalread == 0 && keyDown[keyPos] != 0) {
          // not-shift or windows mode
          if ((keyPos !=16 && keyPos != 58) || windowsShift == 1) {
            // reset keybounce delay
            lastDebounceTime[keyPos] = time;
            // pass key release to windows
            Keyboard.release(keyDown[keyPos]);
          } else {
            // reset keybounce delay and mark as un-shifted
            lastDebounceTime[keyPos] = time;
            shift = 0;
          }
          // set keydown array position as up
          keyDown[keyPos] = 0;
        }
      }
    }
    // set output back to high
    digitalWrite(outPinSet, HIGH);
  }
}
