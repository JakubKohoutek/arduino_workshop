// Constants with RGB diode pins
int RED_PIN = 3;
int GRN_PIN = 5;
int BLU_PIN = 6;

void setup() {
  // Initiating the serial monitor
  Serial.begin(9600);

  // Declaration of pins as output
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(BLU_PIN, OUTPUT);    
}

int readNumberFromSerial(String color) {
  Serial.print(color + " : ");
  // While serial input is not available, do nothing (wait)
  while(!Serial.available()){}
  
  int value = Serial.parseInt();
  if(value >= 0 && value < 256) {
    Serial.println(value);
    return value;
  }

  // If the parsed integer is not within required range, use recursion to
  // try to read the value for current color again
  Serial.println("Invalid input, number must be in interval <0,255>");
  return readNumberFromSerial(color);
}

void lightUpRGB(int red, int grn, int blu) {
  analogWrite(RED_PIN, red);
  analogWrite(GRN_PIN, grn);
  analogWrite(BLU_PIN, blu);
}

void loop() {
  Serial.println("Enter brightness for individual colors:");

  // Variables defining brightness, must be in range <0,255>
  int redBrightness = readNumberFromSerial("Red");
  int grnBrightness = readNumberFromSerial("Green");
  int bluBrightness = readNumberFromSerial("Blue");

  lightUpRGB(redBrightness, grnBrightness, bluBrightness);

  // Wait 5 seconds before asking for new colors again
  delay(5000);
}
