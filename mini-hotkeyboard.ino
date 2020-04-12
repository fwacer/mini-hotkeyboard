/*  mini-hotkeyboard

    AUTHOR: Bryce Dombrowski
    Copyright 2019 - https://brycedombrowski.com
    
    This program takes an array of knobs, switches, and buttons and turns it into a hotkey keyboard for any computer that you plug it into.
*/

//Flags for Encoder.h
#define ENCODER_USE_INTERRUPTS

// Library includes
#include "HID-Project.h"
#include <Encoder.h>

// Define I/O pins
int OUTPUT_PINS[] = { A0,7,8,9,10 };
int INPUT_PINS[] = { 14,15,16 };

// Set up rotary encoders
Encoder knob_1(3,4);
Encoder knob_2(2,5);
volatile long pos_1 = -999;
volatile long pos_2 = -999;

// Extras
bool FLAG_SWITCH1 = false;
bool FLAG_SWITCH2 = false;

void keyPressed(int input_registered, int current_output){
  /* This function takes the output pin that is currently set to HIGH and the input pin that is currently reading HIGH to determine which button has been pressed.
   * Then, a macro or action is taken based on the button pressed.
  */

  /* Keypad layout for reference
   * 
   * A1 B1 C1 D1 --
   * A2 B2 C2 D2 E2
   * A3 B3 C3 D3 E3
   * 
   */
   
  switch(current_output){
    case 0:
    switch(input_registered){
      case 0: // Caps lock (toggle switch)
        // Serial.println("A1");
        FLAG_SWITCH1 = true;
      break;
      case 1: // Increase font size (Word)
        // Serial.println("A2");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.write('>');
      break;
      case 2: // Decrease font size (Word)
        // Serial.println("A3");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.write('<');
      break; 
    }
    break;
    case 1:
    switch(input_registered){
      case 0: // Apply the "Normal Style" in Word (font size knob's button) / Open incognito mode in Chrome
        // Serial.println("B1");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.press(KEY_LEFT_SHIFT);
        BootKeyboard.write('n');
      break;
      case 1: // Paragraph indent (Word)
        // Serial.println("B2");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.write('m');
      break;
      case 2: // Remove paragraph indent (Word)
        // Serial.println("B3");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.write('M');
      break; 
    }
    break;
    case 2:
    switch(input_registered){
      case 0: // Volume mute (volume knob's button)
        // Serial.println("C1");
        Consumer.write(MEDIA_VOLUME_MUTE);
      break;
      case 1: // Heading 1 (Word)
        // Serial.println("C2");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.write('1');
      break;
      case 2: // Bullet point (Word)
        // Serial.println("C3");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.write('L');
      break; 
    }
    break;
    case 3:
    switch(input_registered){
      case 0: // Play star wars IV in ascii
        // Serial.println("D1");
      if (!FLAG_SWITCH2){ // Only play once per flip of switch
        FLAG_SWITCH2 = true;
        
        /*BootKeyboard.press(KEY_LEFT_WINDOWS);
        BootKeyboard.write('r');
        BootKeyboard.releaseAll();
        BootKeyboard.println("telnet towel.blinkenlights.nl");*/
        BootKeyboard.releaseAll();
        BootKeyboard.print("Jenessa Joy Klukas"); // Save time typing their name
      }
      break;
      case 1: // Heading 2 (Word)
        // Serial.println("D2");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.write('2');
      break;
      case 2: // En dash (Word)
        // Serial.println("D3");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.write(KEYPAD_SUBTRACT);
      break; 
    }
    break;
    case 4:
    switch(input_registered){
      /* "E1" does not exist
      case 0: // Function
      // Serial.println("E1");
      break;
      */
      case 1: // Heading 3 (Word)
        // Serial.println("E2");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.write('3');
      break;
      case 2: // Em dash (Word)
        // Serial.println("E3");
        BootKeyboard.press(KEY_LEFT_CTRL);
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.write(KEYPAD_SUBTRACT);
      break; 
    }
    break;
    
  }

  delay(50); // Leave a small amount of time for computer to recognize emulated keys being held
  BootKeyboard.releaseAll();
  delay(100); // Debounce, allows some time between key presses. Future work could include adding proper repeat delay.
}

void checkKnobs(){
  long newPos1, newPos2;
  newPos1 = knob_1.read();
  newPos2 = knob_2.read();
  int threshhold = 1;
  if (newPos1 != pos_1 || newPos2 != pos_2){
 
  if (newPos1>(pos_1+threshhold)){ // Font size up
    BootKeyboard.press(KEY_LEFT_CTRL);
    BootKeyboard.press(']');
    BootKeyboard.releaseAll();
    pos_1 = newPos1;
  }else if (newPos1<(pos_1-threshhold)){ // Font size down
    BootKeyboard.press(KEY_LEFT_CTRL);
    BootKeyboard.press('[');
    BootKeyboard.releaseAll();
    pos_1 = newPos1;
  }
  if (newPos2>(pos_2+threshhold)){ // Volume up
    Consumer.write(MEDIA_VOL_UP);
    pos_2 = newPos2;
  }else if (newPos2<(pos_2-threshhold)){ // Volume down
    Consumer.write(MEDIA_VOL_DOWN);
    pos_2 = newPos2;
  }
  }
  /*
    Serial.print("Left = ");
    Serial.print(newPos1);
    Serial.print(", Right = ");
    Serial.print(newPos2);
    Serial.println();
    pos_1 = newPos1;
    pos_2 = newPos2;*/
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
  for (pin = 0; pin <= 2; pin++) // 0, 1, 2
    pinMode(INPUT_PINS[pin], INPUT);
  for (pin = 0; pin <= 4; pin++) // 0, 1, 2, 3, 4
    pinMode(OUTPUT_PINS[pin], OUTPUT);

  // Set up rotary encoder pins
  for (pin = 2; pin <= 5; pin++) // 2, 3, 4, 5
    pinMode(pin, INPUT);
  
  // Set up keyboard emulators from HID-Project
  BootKeyboard.begin();
  Consumer.begin();
  Serial.begin(9600);
}

void loop() {
  /* This is the event loop of the board.
   * Each output line is toggled sequentially, and all input lines are checked for each toggle.
   * We also force num lock to stay on.
   */
  forceNumLock();
  // Currently disabled because the rotary encoders used don't work
  //checkKnobs(); // Volume and font size control
  
  int output_pin;
  for (output_pin = 0; output_pin <= 4; output_pin++){
    digitalWrite(OUTPUT_PINS[output_pin], HIGH);
    
    // Check input pins for a signal, which would indicate that a button has been pressed.
    int input_pin;
    for (input_pin = 0; input_pin <= 2; input_pin++){ // Check each of pins 0 to 2, inclusive
      if (digitalRead(INPUT_PINS[input_pin]) == HIGH){
        Serial.print(INPUT_PINS[input_pin]);Serial.print(", ");Serial.print(OUTPUT_PINS[output_pin]);Serial.print(" : ");
        keyPressed(input_pin, output_pin); // start at 0
      }else if(output_pin == 0 && input_pin == 0){
        FLAG_SWITCH1 = false; // Caps lock
      }else if(output_pin == 3 && input_pin == 0){
        FLAG_SWITCH2 = false; // Macro
      }
    } // for
    
    digitalWrite(OUTPUT_PINS[output_pin], LOW); // Turn off output pin
    
    if (bool(BootKeyboard.getLeds() & LED_CAPS_LOCK) != FLAG_SWITCH1){
      BootKeyboard.write(KEY_CAPS_LOCK); // Toggle caps lock if the computer's state does not match the state requested
      delay(20);
    }/*
    if (bool(BootKeyboard.getLeds() & LED_NUM_LOCK) != FLAG_NUM_LOCK){
      BootKeyboard.write(KEY_NUM_LOCK); // Toggle num lock if the computer's state does not match the state requested
      delay(20);
    }*/
  } // for
} // loop
