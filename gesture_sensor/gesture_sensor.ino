#include <Wire.h>
#include <SparkFun_APDS9960.h>

// Constants with RGB diode pins
const int RED_PIN = 3;
const int GRN_PIN = 5;
const int BLU_PIN = 6;

// Constants for interrupt pin the sensor is connected to
const int INTERRUPT_PIN = 2;
const int INTERRUPT_IDX = digitalPinToInterrupt(INTERRUPT_PIN);

// Constant for max brigthness, must be divisible by 3
int MAX_BRIGHTNESS = 240;

// Constant With all possible colors
String COLORS [] = {
  "red",
  "green",
  "blue",
  "white",
  "pink"
};

// Sensor variable - instance of SparkFun_APDS9960 class
SparkFun_APDS9960 apds = SparkFun_APDS9960();

// Shared flag variable for interrupt function
volatile bool interruptFlag = false;

// Variables for current color and brightness
int currentColor = 0;
int currentBrightness = MAX_BRIGHTNESS;

void setup() {
  // Initiating the serial monitor
  Serial.begin(9600);

  // Set interrupt pin as input and attach interrupt event => once the sensor
  // registers something in proximity, it will connect the interrupt pin
  // to ground and the onInterrupt function will be executed
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(INTERRUPT_IDX, onInterrupt, FALLING);

  // Declaration of pins as output
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(BLU_PIN, OUTPUT);

  // Initialize APDS-9960 sensor
  if (apds.init() && apds.enableGestureSensor(true)) {
    Serial.println("Gesture sensor is now running");
  } else {
    Serial.println("Something went wrong during gesture sensor init!");
  }
}

void loop() {
  if(interruptFlag) {
    handleGesture();
    interruptFlag = false;
  }
  displayCurrentColor();
}

void switchToNextColor () {
  int colorsLength = sizeof(COLORS) / sizeof(COLORS[0]);
  currentColor = (currentColor + 1 >= colorsLength) ? 0 : currentColor + 1;
}

void switchToPrevColor() {
  int colorsLength = sizeof(COLORS) / sizeof(COLORS[0]);
  currentColor = (currentColor - 1 < 0) ? colorsLength - 1 : currentColor - 1;
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

void lightUpRGB(int red, int grn, int blu) {
  analogWrite(RED_PIN, red * currentBrightness / 255);
  analogWrite(GRN_PIN, grn * currentBrightness / 255);
  analogWrite(BLU_PIN, blu * currentBrightness / 255);
}

void displayCurrentColor() {
  if(COLORS[currentColor] == "red")
    lightUpRGB(255,   0,   0);
  else if(COLORS[currentColor] == "green")
    lightUpRGB(  0, 255,   0);
  else if(COLORS[currentColor] == "blue")
    lightUpRGB(  0,   0, 255);
  else if(COLORS[currentColor] == "white")
    lightUpRGB(255, 255, 255);
  else if(COLORS[currentColor] == "pink")
    lightUpRGB(255,  96, 208);
}

void onInterrupt() {
  // We set the interrupt flag so that we know that in the next loop cycle
  // we should hanlde the gesture
  interruptFlag = true;
}

void handleGesture() {
  // Detach the interrupt - we don't want to add new gestures to the queue until
  // the current one is processed
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
      default:
        Serial.println("NONE");
    }
  }

  // Attach the interrupt again
  attachInterrupt(INTERRUPT_IDX, onInterrupt, FALLING);
}
