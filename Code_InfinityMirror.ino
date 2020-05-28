/*
 * Code for controlling the LED-Stip in a Infinity Mirror using four buttons. 
 * Toggle between colors/animations, pause/unpause animations and change brightness.
 * Based on a Atmega328P-AU MCU
 * 
 * 

  Pin Configuration:

  D3 = Left Button   ( Input Pullup )
  D2 = Mode Button   ( Input Pullup )
  D4 = Config Button ( Input Pullup )
  D5 = Right Button  ( Input Pullup ) 
  D6 = Data Pin 
  ( D7 = Clock Pin )


  Configuration for LED-Strip without CLK:

  - // #define CLK_PIN   7
  - // FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  -    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  - pinMode(7, INPUT_PULLUP);


  Configuration for LED-Strip with CLK:
  
  - #define CLK_PIN   7
  - FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  - // FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  - // pinMode(7, INPUT_PULLUP);



  Button Description:

    OFF Mode:  
              - Left Button   => Wake up to Cycle Animation Mode
              - Mode Button   => Wake up to Cycle Animation Mode
              - Config Button => Wake up to Cycle Animation Mode
              - Right Button  => Wake up to Cycle Animation Mode

    Cycle Animation Mode:  
              - Left Button   => Decrease Brightness
              - Mode Button   => Constant Animation Mode
              - Config Button => Pause Animation Mode
              - Right Button  => Increase Brightness

    Constant Animation Mode:   
              - Left Button   => Decrease Brightness
              - Mode Button   => OFF Mode
              - Config Button => Next Animation
              - Right Button  => Increase Brightness

    Pause Animation Mode:    
              - Left Button   => Decrease Brightness
              - Mode Button   => Cycle Animation Mode
              - Config Button => Unpause Animation
              - Right Button  => Increase Brightness
*/


#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define FRAMES_PER_SECOND  30
#define LED_TYPE    WS2812  // Type of LED in LED-Strip
#define COLOR_ORDER GRB
#define NUM_LEDS    46  // Number of LED:s in LED-Strip
#define DATA_PIN    6
//#define CLK_PIN   7

CRGB leds[NUM_LEDS];

const int bUpButton = 5; // Increase brightness
const int modeButton = 3; // Change Mode
const int configButton = 4; // Switch Animation
const int bDownButton = 2;  // Decrease Brightness

// Start Brightness
int BRIGHTNESS = 105;

void setup() {
  delay(1000); // delay for recovery

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);

  // Used pins configuration
  pinMode(bUpButton, INPUT_PULLUP);
  pinMode(bDownButton, INPUT_PULLUP);
  pinMode(modeButton, INPUT_PULLUP);
  pinMode(configButton, INPUT_PULLUP);

  // Unused pins pulled high
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { Rainbow, Confetti, Sinelon, Juggle, Bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop() {

  // Cycle Animation Mode

  BrightnessAdjust();

  if (ButtonPress(modeButton) == true) {
    ConstantAnimation();
  }

  if (ButtonPress(configButton) == true) {
    PauseAnimation();
  }
  UpdateAnimation();
  NextAnimation();
}

bool ButtonPress(int button) {
  if (digitalRead(button) == LOW) {
    while (digitalRead(button) == LOW) {
      // Wait for button release 
    }
    return true;
  }
  return false;
}

void UpdateAnimation() {
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  return;
}

void NextAnimation() {
  EVERY_N_SECONDS( 10 ) {
    NextPattern();  // change patterns periodically
  }
  return;
}

void PauseAnimation() {
  while (ButtonPress(configButton) == false) {
    BrightnessAdjust();

    if (ButtonPress(modeButton) == true) {
      Off();
      return;
    }
  }
  return;
}

void ConstantAnimation() {
  NextPattern(); // Go to next pattern to indicate mode change
  while(ButtonPress(modeButton) == false) {
    UpdateAnimation();
    BrightnessAdjust();

    if (ButtonPress(configButton) == true) {
      NextPattern();
    }
  }

  Off();
  return;
}

void Off() {
  FastLED.setBrightness(0);
  FastLED.show();
  
  while (ButtonPress(modeButton) == false && ButtonPress(configButton) == false && ButtonPress(bUpButton) == false && ButtonPress(bDownButton) == false) {
    // Wait for a button press
  }

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  return;
}

void BrightnessAdjust() {

  if (ButtonPress(bUpButton) == true) {

    if (BRIGHTNESS > 245) {
      BRIGHTNESS = 245;
      BrightnessLimit();
    } else {
      BRIGHTNESS += 25;
    }
    
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
  }

  if (ButtonPress(bDownButton) == true) {

    BRIGHTNESS -= 25;

    if (BRIGHTNESS <= 5) {
      BRIGHTNESS = 5;
      BrightnessLimit();
    }
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
  }
  return;
}

void BrightnessLimit() {
  FastLED.setBrightness(0);
  FastLED.show();
  delay(80);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
  return;
}

void NextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  return;
}

void Rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  return;
}

void Confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  return;
}

void Sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
  return;
}

void Juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  return;
}

void Bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  return;
} 
