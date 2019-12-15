/*  mini-hotkeyboard

    AUTHOR: Bryce Dombrowski
    Copyright 2019 - https://brycedombrowski.com
    
    This program takes an array of knobs, switches, and buttons and turns it into a hotkey keyboard for any computer that you plug it into.
*/

// Library includes
#include "HID-Project.h"
#include <Encoder.h>

// Define I/O pins
#define OUTPUT_1     6
#define OUTPUT_2     7
#define OUTPUT_3     8
#define OUTPUT_4     9
#define OUTPUT_5     10

#define INPUT_1     11
#define INPUT_2     12
#define INPUT_3     13

// Set up rotary encoders
Encoder knob_1(3,5);
Encoder knob_2(2,4);
long position_knob_1  = 0L;
long position_knob_2  = 0L;

// Extras
bool FLAG_CAPS_LOCK = false:

void keyPressed(int input_registered, int current_output){
  /* This function takes the output pin that is currently set to HIGH and the input pin that is currently reading HIGH to determine which button has been pressed.
   * Then, a macro or action is taken based on the button pressed.
  */

  /* Keypad layout for reference
   * 
   * A1 B1 C1
   * A2 B2 C2
   * A3 B3 C3
   * A4 B4 C4
   * A5 B5 C5  <-- switch + two buttons on knobs
   */
   
  switch(input_registered){
    case 0:
    switch(current_output){
      case 0: // Function
      // Serial.println("A1");
      
      break;
      case 1: // Function
      // Serial.println("A2");
      
      break;
      case 2: // Function
      // Serial.println("A3");
      
      break; 
      case 3: // Function
      // Serial.println("A4");
      
      break; 
      case 4: // Caps lock (toggle switch)
      // Serial.println("A5");
      FLAG_CAPS_LOCK = false;
      break; 
    }
    break;
    case 1:
    switch(input_registered){
      case 0: // Function
      // Serial.println("B1");
      
      break;
      case 1: // Function
      // Serial.println("B2");
      
      break;
      case 2: // Function
      // Serial.println("B3");
      
      break; 
      case 3: // Function
      // Serial.println("B4");
      
      break; 
      case 4: // Apply the "Normal Style" in Word (font size knob's button) / Open incognito mode in Chrome
      // Serial.println("B5");
      BootKeyboard.press(KEY_LEFT_CTRL);
      BootKeyboard.press(KEY_LEFT_SHIFT);
      BootKeyboard.write('n');
      break; 
    }
    break;
    case 2:
    switch(input_registered){
      case 0: // Function
      // Serial.println("C1");
      
      break;
      case 1: // Function
      // Serial.println("C2");
      
      break;
      case 2: // Function
      // Serial.println("C3");
      
      break; 
      case 3: // Function
      // Serial.println("C4");
      
      break; 
      case 4: // Volume mute (volume knob's button)
      // Serial.println("C5");
      Consumer.write(MEDIA_VOLUME_MUTE);
      break; 
    }
    break;
  }

  delay(50); // Leave a small amount of time for computer to recognize emulated keys being held
  BootKeyboard.releaseAll();
  delay(100); // Debounce, allows some time between key presses. Future work could include adding a repeat delay.
}

void checkKnobs(){
  long new1 = knob_1.read();
  long new2 = knob_2.read();
  long i = 0L;
  if (new1 != position_knob_1) {
    long delta = abs(new1 - position_knob_1);
    if (new1 > position_knob_1){
      // Volume up
      //rotated right
      for(i = 0; i < delta; i++) // Hold for time proportional to amount turned
        Consumer.write(MEDIA_VOL_UP);
    }else{
      // Volume down
      //rotated left
      for(i = 0; i < delta; i++) // Hold for time proportional to amount turned
        Consumer.write(MEDIA_VOL_DOWN);
    }
    position_knob_1 = new1;
  }
  if (new2 != position_knob_2){
    long delta = abs(new2 - position_knob_2);
    if (new2 > position_knob_2){
      //rotated right
      // Font size up (in Word)
      
      BootKeyboard.press(KEY_LEFT_CTRL);
      BootKeyboard.press(']');
      for(i = 0; i < delta; i++)
        delay(50); // Hold for time proportional to amount turned [may need to instead repeat the hold/release combo instead of holding]
      BootKeyboard.releaseAll();
      }
    }else{
      //rotated left
      // Font size down (in Word)
      BootKeyboard.press(KEY_LEFT_CTRL);
      BootKeyboard.press('[');
      for(i = 0; i < delta; i++)
        delay(50); // Hold for time proportional to amount turned [may need to instead repeat the hold/release combo instead of holding]
      BootKeyboard.releaseAll();
    }
    position_knob_2 = new2;
  }
}

void forceNumLock(){
  /* Forces NumLock to stay on. The ALT keycodes used by the keypad buttons wouldn't work otherwise.
  */
  if (!(BootKeyboard.getLeds() & LED_NUM_LOCK))
    BootKeyboard.write(KEY_NUM_LOCK);  
}

void setup() {
  /* This function sets up digital I/O pins and starts the keyboard emulator.
   */
  // Set up digital I/O pins
  int pin;
  for (pin = INPUT_1; pin <= INPUT_3; pin++)
    pinMode(pin, INPUT);
  for (pin = OUTPUT_1; output_pin <= OUTPUT_5; output_pin++)
    pinMode(pin, OUTPUT);
  
  // Set up keyboard emulators from HID-Project
  BootKeyboard.begin();
  Consumer.begin();
}

void loop() {
  /* This is the event loop of the board.
   * Each output line is toggled sequentially, and all input lines are checked for each toggle.
   * We also force num lock to stay on.
   */
  forceNumLock();
  checkKnobs(); // Volume and font size control
  
  int output_pin;
  for (output_pin = OUTPUT_1; output_pin <= OUTPUT_5; output_pin++){
    digitalWrite(output_pin, HIGH);
    
    delay(1); // Probably unnecessary
    // Check input pins for a signal, which would indicate that a button has been pressed.
    int input_pin;
    for (input_pin = INPUT_1; input_pin <= INPUT_3; input_pin++){
      if (digitalRead(input_pin) == HIGH)
        keyPressed(input_pin-INPUT_FIRST, output_pin-OUTPUT_FIRST); // start at 0
    } // for
    
    digitalWrite(output_pin, LOW); // Turn off output pin
    
    if (bool((BootKeyboard.getLeds() & LED_CAPS_LOCK)) != FLAG_CAPS_LOCK)
      BootKeyboard.write(KEY_CAPS_LOCK); // Toggle caps lock if the computer's state does not match the state requested
  } // for
} // loop
