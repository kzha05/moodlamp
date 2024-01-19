// TITLE: A NeoPixel LED strip program.

// SETUP: Add dependencies
#include <Adafruit_NeoPixel.h> // LED strip dependency
#ifdef __AVR__
 #include <avr/power.h>
#endif

// SETUP: Define the main pin you use from the arduino to the LED strip
#define LED_PIN 6
int touch = 9;  // pin for touch sensor
int touch1 = 10;
int ledPin = 6; // pin for the LED

// SETUP: How many pixels does the LED strip have? (the full one was 120, the halves are 60 each)
#define LED_COUNT 120

// SETUP: LED strip
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// SETUP: Define the list array as a global variable & define lastColor as global variable
uint32_t list[7];
int lastColorIndex = -1;
long firstPixelHue = 0;

// FUNC + SETUP: LED strip, void setup() runs once at the start of the program.
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(touch, INPUT);
  pinMode(touch1, INPUT);
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin(); // IMPORTANT: Starts running the LED strip
  strip.show(); // SETUP: Turn off all pixels
  strip.setBrightness(50); // SETUP: Set brightness to 50, max = 255
  Serial.begin(9600); // SETUP: Setting the Serial baudrate to 9600 incase you ever want to print something
  randomSeed(analogRead(0)); // SETUP: Arduino likes remembering stuff, this makes sure it starts clean

   // SETUP: fill the list array
  list[0] = strip.Color(255, 0, 0);      // DEVEX: red
  list[1] = strip.Color(0, 255, 0);      // DEVEX: green
  list[2] = strip.Color(0, 0, 255);      // DEVEX: blue
  list[3] = strip.Color(255, 0, 255);    // DEVEX: purple
  list[4] = strip.Color(255, 255, 0);    // DEVEX: yellow
  list[5] = strip.Color(153, 76, 0);     // DEVEX: orange
  list[6] = strip.Color(255, 255, 255);  // DEVEX: white
}


// EXPL: loop() function -- runs repeatedly as long as board is on

void loop() {
  int touchValue = digitalRead(touch);
  int touchValue1 = digitalRead(touch1);
  if (touchValue == HIGH || touchValue1 == HIGH){
    digitalWrite(ledPin, HIGH);
    Serial.println("touched");
    rainbow(0.1,3000);
    theaterChaseRainbow(50, 10); // delay 50ms, repeat 30 times
    colorWipe(randomColorFromList(list, 7), 50);
    delay(5000);
  }
  else{
    digitalWrite(ledPin,LOW);
    rainbow(20, 100);
    Serial.println("not touched");
  }
}

// DEVEX: Helper functions

// FUNC: Returns a random color
// EXAMPLE: colorWipe(randomColor(), 50)
uint32_t randomColor(void) {
  return strip.Color(random(32, 150), random(32, 150), random(32, 150));
}

// FUNC: Returns a random color from list
// ARGS: list: array of colors, size: size of array
// EXAMPLE: colorWipe(randomColorFromList(list,8), 50);
uint32_t randomColorFromList(uint32_t* list, int size) {
  int newColorIndex = random(0, size - 1);

  // DEVEX: Ensure the new color is different from the last one
  while (newColorIndex == lastColorIndex) {
    newColorIndex = random(0, size - 1);
  }

  lastColorIndex = newColorIndex;
  return list[newColorIndex];
}

// DEVEX: Effect functions

// URTURN: Document what this function does
// ARGS: color in strip.Color(r,g,b), wait in ms
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show(); // EXPL: Update strip
    delay(wait);
  }
}

// URTURN: Document what this function does
// ARGS: color in strip.Color(r,g,b), wait in ms, repeat in amount of times it repeats
void theaterChase(uint32_t color, int wait, int repeat) {
  for(int a=0; a<repeat; a++) {
    for(int b=0; b<3; b++) {
      strip.clear(); // EXPL: Turn all pixels off
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // EXPL: Set pixel 'c' to value 'color'
      }
      strip.show();
      delay(wait);
    }
  }
}

// URTURN: Document what this function does
// ARGS wait in ms, duration in ms
void rainbow(int wait, int duration) {
  unsigned long startTime = millis(); 

    if (firstPixelHue >= 327424){
      firstPixelHue = 0;
    }
    for (long tempFirstPixelHue = firstPixelHue; tempFirstPixelHue < 5 * 65536; tempFirstPixelHue += 256) {
      Serial.println(tempFirstPixelHue);
      firstPixelHue = tempFirstPixelHue;
      if (millis() - startTime >= duration) {
        break;
      }
      strip.rainbow(tempFirstPixelHue);
      strip.show(); // Update strip
      delay(wait);
    }
}

// URTURN: Document what this function does
// ARGS: color in strip.Color(r,g,b), wait in ms, repeat in amount of times it repeats
void theaterChaseRainbow(int wait, int repeat) {
  int tempFirstPixelHue = 0;     // EXPL: First pixel starts at red (hue 0)
  for(int a=0; a<repeat; a++) {
    for(int b=0; b<3; b++) {
      strip.clear(); // EXPL: Turn all pixels off

      // EXPL: function to do rainbow voodoo
      for(int c=b; c<strip.numPixels(); c += 3) {
        int      hue   = tempFirstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // DEVEX: hue -> RGB
        strip.setPixelColor(c, color); // EXPL: Set pixel 'c' to value 'color'
      }
      
      strip.show(); // EXPL: Update strip
      delay(wait);
      tempFirstPixelHue += 65536 / 90; // EXPL: One cycle of color wheel over 90 frames
    }
  }
}
