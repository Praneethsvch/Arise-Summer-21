#include <Wire.h>
#include "HT_SH1107Wire.h"
#include "HT_DisplayUi.h"
#include "images.h"
#include "softSerial.h"

#define CONTROL_PIN 21   // This is the YELLOW wire, can be any data line

uint16_t distance;  // The last measured distance
bool newData = false; // Whether new data is available from the sensor
uint8_t buffer[4];  // our buffer for storing data
uint8_t idx = 0;  // our idx into the storage buffer
uint16_t distanceAdafruit = 0;  // The last measured distance
bool newDataAdafruit = false; // Whether new data is available from the sensor
uint8_t bufferAdafruit[4];  // our buffer for storing data
softSerial softwareSerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);
SH1107Wire  display(0x3c, 500000, SDA, SCL , GEOMETRY_128_64, GPIO10); // addr, freq, sda, scl, resolution, rst
DisplayUi ui( &display );
uint16_t medianOfReadings = 0;
uint16_t readings_arr[20] = {0};
size_t n = *(&readings_arr + 1) - readings_arr;
unsigned int sensorMode = 2;
unsigned int sensor_sampling_rate = 250;
unsigned int sensor_numberOfReadings = 5;

void getDistanceAdafruit()
{
  if (softwareSerial.available()) {
    uint8_t c = softwareSerial.read();
    // See if this is a header byte
    if (idx == 0 && c == 0xFF) {
      bufferAdafruit[idx++] = c;
    }
    // Two middle bytes can be anything
    else if ((idx == 1) || (idx == 2)) {
      bufferAdafruit[idx++] = c;
    }
    else if (idx == 3) {
      uint8_t sum = 0;
      sum = bufferAdafruit[0] + bufferAdafruit[1] + bufferAdafruit[2];
      if (sum == c) {
        distanceAdafruit = ((uint16_t)bufferAdafruit[1] << 8) | bufferAdafruit[2];
        newDataAdafruit = true;
      }
      idx = 0;
    }
  }
}

void displayReading(ScreenDisplay *display, DisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 20 + y, "Distance:");
  for (int i = 0; i <= 3; i++) {
    while (newDataAdafruit == false || distanceAdafruit == 0) {
      getDistanceAdafruit();
    }
    newDataAdafruit = false;

    delay(100);
  }
  Serial.println(distanceAdafruit);
  //medianOfReadings = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, 34 + y, (String) distanceAdafruit);
}

void VextON(void)
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { displayReading};

// how many frames are there?
int frameCount = 1;



void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  VextON();
  delay(100);
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Initialising the UI will init the display too.
  ui.init();

  softwareSerial.begin(9600);
  pinMode(CONTROL_PIN, OUTPUT);
  digitalWrite(CONTROL_PIN, HIGH);
}

void loop() {
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
