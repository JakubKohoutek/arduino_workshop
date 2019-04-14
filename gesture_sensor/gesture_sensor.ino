#include <Wire.h>
#include <SparkFun_APDS9960.h>

// Constants with RGB diode pins
#define RED_PIN    3
#define GRN_PIN    5
#define BLU_PIN    6

// Constant for interrupt pin the sensor is connected to
#define INT_PIN    2

// Initiate ADPS-9960 sensor
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

// Constant for max brigthness, must be divisible by 3
int MAX_BRIGHTNESS = 240;

// Define all possible colors
String colors [] = {
  "red",
  "green",
  "blue",
  "white",
  "pink"
};

// Global variables for current color and current brightness
int currentColor = 0;
int currentBrightness = MAX_BRIGHTNESS;


void setup() {
  // Initiating the serial monitor
  Serial.begin(9600);

  // Set interrupt pin as input
  pinMode(INT_PIN, INPUT_PULLUP);

  // Declaration of pins as output
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(BLU_PIN, OUTPUT);

  // Declaration of interrupt pin for sensor
  pinMode(INT_PIN, INPUT_PULLUP);

  // Initialize APDS-9960 (configure I2C and initial values)
  attachInterrupt(0, onInterrupt, FALLING);
  if (apds.init()) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Start the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}

void lightUpRGB(int red, int grn, int blu) {
  analogWrite(RED_PIN, red * currentBrightness / 255);
  analogWrite(GRN_PIN, grn * currentBrightness / 255);
  analogWrite(BLU_PIN, blu * currentBrightness / 255);
}

void switchToNextColor () {
  int colorsLength = sizeof(colors) / sizeof(colors[0]);
  currentColor = currentColor + 1 >= colorsLength ? 0 : currentColor + 1;
}

void switchToPrevColor() {
  int colorsLength = sizeof(colors) / sizeof(colors[0]);
  currentColor = currentColor - 1 < 0 ? colorsLength - 1 : currentColor - 1;
}

void increaseBrightness() {
  int brightnessStep = MAX_BRIGHTNESS / 3;
  if(currentBrightness == MAX_BRIGHTNESS)
    return;
  currentBrightness += brightnessStep;
}

void decreaseBrightness() {
  int brightnessStep = MAX_BRIGHTNESS / 3;
  if(currentBrightness == 0)
    return;
  currentBrightness -= brightnessStep;
}

void onInterrupt() {
  isr_flag = 1;
}

void loop() {
  if( isr_flag == 1 ) {
    detachInterrupt(0);
    if ( apds.isGestureAvailable() ) {
      switch ( apds.readGesture() ) {
        case DIR_UP:
          Serial.println("UP");
          increaseBrightness();
          break;
        case DIR_DOWN:
          Serial.println("DOWN");
          decreaseBrightness();
          break;
        case DIR_LEFT:
          Serial.println("LEFT");
          switchToPrevColor();
          break;
        case DIR_RIGHT:
          Serial.println("RIGHT");
          switchToNextColor();
          break;
        case DIR_NEAR:
          Serial.println("NEAR");
          break;
        case DIR_FAR:
          Serial.println("FAR");
          break;
        default:
          Serial.println("NONE");
      }
    }
    isr_flag = 0;
    attachInterrupt(0, onInterrupt, FALLING);
  }

  if(colors[currentColor] == "red")
    lightUpRGB(255,   0,   0);
  else if(colors[currentColor] == "green")
    lightUpRGB(  0, 255,   0);
  else if(colors[currentColor] == "blue")
    lightUpRGB(  0,   0, 255);
  else if(colors[currentColor] == "white")
    lightUpRGB(255, 255, 255);
  else if(colors[currentColor] == "pink")
    lightUpRGB(255,  96, 208);
}
