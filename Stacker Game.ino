//Stacker Game
// C++ code

#include "Timer.h"

// Helper Function display a number on ONE digit

// gSegPins
// An array of pins of the arduino that are connected
// to segments a, b, c, d, e... g in that order.
char gSegPins[] = { A4, A3, 6, 5, 4, A2, 7};


// displayNumTo7Seg
// displays one number (between 0 and 9) "targetNum" on the digit conneceted to "digitPin"
// E.g. If I wanted to display the number 6 on the third digit of my display.
// and the third digit was connected to pin A0, then I'd write: displayNumTo7Seg(6, A0);
void displayNumTo7Seg(unsigned int targetNum, int digitPin) {


    // A map of integers to the respective values needed to display
    // a value on one 7 seg digit.
    unsigned char encodeInt[10] = {
        // 0     1     2     3     4     5     6     7     8     9
        0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    };




    // Make sure the target digit is off while updating the segments iteratively
    digitalWrite(digitPin, HIGH);


    // Update the segments
    for (int k = 0; k < 7; ++k) {
        digitalWrite(gSegPins[k], encodeInt[targetNum] & (1 << k));
    }


    // Turn on the digit again
    digitalWrite(digitPin, LOW);
}
  
/////////////

//char gSegPins[] = { 2, 3, 4, 5, 6, 7 };
char digitPins[] = {2, 3, };
char ledPins[] = {9, 10, 11, 12, 13};
int ledDirection = 1;        // tracks the direction of the LED, 1(right), -1(left)
unsigned char currentDigit = 0;

// global variables here
enum LEDstates { START, SHIFT, HOLD_LED }; LED_state;
enum ButtonState { WAIT, PRESS, RELEASE }; B_state;

struct Stacker {
    unsigned char LEDpos;
    unsigned char buttonPress;
    unsigned int score;

    // void (*tick) ( void ); idk what this does

    LEDstates ledState;
    ButtonState buttonState;
};

Stacker game;

// function declarations
void tick_LEDstates(Stacker &game);
void tick_buttonState(Stacker &game);
void tick_Score(Stacker &game);
void updateLED(unsigned char pos);
void displayNumTo7Seg(unsigned int targetNum, int digitPin);

void setup() {
    game.ledState = START;
    game.buttonState = WAIT;

    game.LEDpos = 0;
    game.buttonPress = 0;
    game.score = 0;

    for (int i = 0; i < 7; i++) {
        pinMode(gSegPins[i], OUTPUT);
    }
    for (int i = 0; i < 4; i++) {
        pinMode(digitPins[i], OUTPUT);
    }
    for (int i = 0; i < 5; i++) {
      pinMode(ledPins[i], OUTPUT);
    }

    //TimerSet(120);
    //TimerOn();

    pinMode(A0, INPUT_PULLUP);
}

volatile unsigned char TimerFlag = 0; // debugger says to put volatile?

void loop() {
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 5;
unsigned long B_elapsedTime = 10;
unsigned long LED_elapsedTime = 50;
const unsigned long timerPeriod = 100;
con
TimerSet(timerPeriod);
TimerOn();
LED_state = START;
B_state = WAIT;
  while(1) {
    TimerFlag = 0;
    while(!TimerFlag) {}
    if (B_elapsedTime >= 10) {
      tick_buttonState();
      tick_Score();
      B_elapsedTime = 0;
    }
    if (LED_elapsedTime >= 120) {
      tick_LEDstates();
      LED_elapsedTime = 0;
    }

    B_elapsedTime += timerPeriod;
    LED_elapsedTime += timerPeriod;
  }

  
  game.buttonPress = digitalRead(A0);

  tick_buttonState(game);
  tick_Score(game);
  tick_LEDstates(game);
  updateLED(game.LEDpos);
}


// update our LED, by position
void updateLED(unsigned char pos) {
    // wipe our leds
    for (int i = 0; i < 5; i++) {
        digitalWrite(ledPins[i], LOW);
    }

    // set by position
    digitalWrite(ledPins[pos], HIGH);
}

void updateScoreDisplay(unsigned short score) {
    unsigned char digits[2] = { score % 10, (score / 10) % 10}; // Split score into two digits

    // Turn off all digit pins initially
    for (int i = 0; i <= 1; i++) {
        digitalWrite(digitPins[i], HIGH); 
    }

    digitalWrite(digitPins[currentDigit], LOW); 
    displayNumTo7Seg(digits[currentDigit], digitPins[currentDigit]);

    // Update currentDigit to point to the next digit
    currentDigit = (currentDigit + 1) % 2;
}

void tick_LEDstates(Stacker &game) {
    // transitions
    switch (game.ledState) {
    case START:
        game.ledState = SHIFT;
        break;

    case SHIFT:
        if (game.buttonPress) {
            game.ledState = HOLD_LED;
        }
        break;

    case HOLD_LED:
        if (!game.buttonPress) {
            // Transition back to SHIFT state if button is released
            game.ledState = SHIFT;
        }
        break;
    }

    // actions
    switch (game.ledState) {
    case START:
        game.LEDpos = 0; // First LED
        break;
    case SHIFT:
        game.LEDpos += ledDirection;

        if (game.LEDpos <= 0 || game.LEDpos >= 4) {
            ledDirection = -ledDirection;
        }
        break;

    case HOLD_LED:

        break;
    }

    updateLED(game.LEDpos);
}


void tick_buttonState(Stacker &game) {
    // transitions + actions (I think it would be unecessary to have both?)
    switch (game.buttonState) {
    case WAIT:
        if (game.buttonPress) {
            game.buttonState = PRESS;
        }
        break;
    case PRESS:
        // check if button is released
        if (!game.buttonPress) {
            game.buttonState = RELEASE;
        }
        break;
    case RELEASE:
        // transitions back to WAIT immediately after release.
        game.buttonState = WAIT;
        break;
    }
}

void tick_Score(Stacker &game) {
    // update score when:  
    // HOLD_LED: we were holding down the button at last tick
    // and RELEASE: button releasing this tick
    if (game.ledState == HOLD_LED && game.buttonState == RELEASE) {
        // add score
        if (game.LEDpos == 2) {
            game.score++;
            if (game.score > 99) {
                game.score = 99;
            }
        }
        else {
            game.score = 0;
        }
    }
    updateScoreDisplay(game.score);
}
