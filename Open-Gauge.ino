/*
 * Open-Gauge v0.14
 * 
 * This is very much a work in progress right now and will only work for positive boost applications at this time (diesel).
 * Once testing is completed with positive boost applications I'll look to add negative for petrol turbo applications.
 * 
 * The entire UI has been written specifically for a SSD1306 128x64 OLED display using I2C. If you don't use this display size then the UI will be messed up and you'll have to re-format everything.
 * Additionally SPI is not catered for as I only have I2C displays on hand.
 * 
 * Remember that display sizes need to be changed in Adafruit_SSD1306.h to get this to work correctly.
 * The BMP280 sensor address also needs changing in Adafruit_BMP280.h if yours is not at the default I2C address.
 * 
 */

float boost = 0;
float atmopsi = 0;
float warnpsi = 35; //35PSI of boost is quite a lot! We probably want to warn if we are going over this. Change this to your maximum boost value application per turbo
int uishown = 0;
float inittemp = 0;

unsigned long lastrunmillis;
unsigned long currentmillis;
const unsigned long sampleperiod1 = 30000; //This sample is used for the temperature

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

Adafruit_BMP280 bme; // I2C


void setup() {
  lastrunmillis = millis();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c); //Initialise at a different I2C address if your display isn't on 0x3c
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.print(F("Open-Gauge"));
  display.println(F("v0.14"));
  display.display();
  delay(2000);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(F("Checking sensors;"));
  display.display();
  
  if (!bme.begin()) {  
   display.setCursor(0,10);
   display.println(F("No BMP280 sensor"));
   display.display();
    while (1);
  }
  
  display.setCursor(0,10);
  display.print(F("BMP280 = OK"));
  display.display();
  delay(1000);
  display.clearDisplay();
  
  //Set a start temp so the temps dont load up entirely blank!
  inittemp = bme.readTemperature();
  display.fillRect(2,2, 62,7, BLACK); //Testing drawing over only areas we have to rather than blanking the whole screen every time
  display.setTextSize(1);
  display.setCursor(15,2);
  display.print(inittemp,1); //Display to 1 decimal place
  display.print((char)247); //Use a proper degrees symbol
  display.print(F("C"));
}

void loop() {
  
  // Set up some sensors
  atmopsi = ((float)bme.readPressure()*0.0001450377); // Create atmospheric pressure PSI for later. We will need it to 0 the boost pressure at altitude etc. Hence BMP280
  boost = ((float)bme.readPressure()*0.0001450377); // Use barometric pressure to PSI to create fake but live PSI for layout testing
  currentmillis = millis(); //This will be used for time based refreshes in functions
  
  showui(); //Start display stuff here
  interiortemp();
  voltmeter();
  boostgauge();
  boostpressbar();
 
  // Now show everything we did
  display.display();

  // Now clear it so we can start again
  //display.clearDisplay(); // Maybe this is a very "expensive" way of drawing the UI and output?

}

void showui () {
  if (uishown >= 1) // Stop us re-drawing the whole UI all the time
  {
  }
  else
  {
  uishown = 1; // If we revert to clearing the entire display, everything up to the void needs to be removed
  display.drawRect(0,0, 128, 64, WHITE); // Location, Size, Colour
  display.drawFastHLine(0,10, 128, WHITE); // Location, Length, Colour
  display.drawFastVLine(64,0, 10, WHITE); // Location, Height, Colour
  }
}

void interiortemp() {
  
  if (currentmillis - lastrunmillis >= sampleperiod1) { // Changed to sample only every sampleperiod1 seconds to stop the constant "flicker" of temperature values. They still move around, but this is nicer.
  // Read temp a few times to "smooth" the values a bit.
  int i;
  float tempavg = 0;

  for (i = 0; i < 30; i++){
    tempavg = tempavg + bme.readTemperature();
  }
  tempavg = tempavg / 30;
  lastrunmillis = currentmillis;
  display.fillRect(2,2, 62,7, BLACK); // Testing drawing over only areas we have to rather than blanking the whole screen every time
  display.setTextSize(1);
  display.setCursor(15,2);
  display.print(tempavg,1); //Display to 1 decimal place
  display.print((char)247); //Use a proper degrees symbol
  display.print(F("C"));
  }
  else
  {
  
  }
}

void voltmeter() {
  display.fillRect(65,2, 62,7, BLACK); // Testing drawing over only areas we have to rather than blanking the whole screen every time
  display.setTextSize(1);
  display.setCursor(80,2);
  display.print(bme.readAltitude(1013.25),0); // Replace altimeter with voltage readout at a later date. Altimeter is just for testing UI layout
  display.setCursor(120,2);
  display.print(F("m"));
}

void boostgauge() {
  display.setCursor(12,16);
  display.setTextSize(2);
  if (boost >= 0.1) {
  display.print(F("PSI")); // Example display layout untill pressure sensor is wired in. Probably want to account for negative pressure at some point incase it's used on a petrol
  }
  else
  {
  display.print(F("VAC"));
  }
  
  if (boost < warnpsi) {
  display.setCursor(69,16);
  display.print(boost,1);
  display.fillRect(12,50, 104,7, BLACK); //Clear out the warning text because we returned below warning PSI levels
  }
  else
  {
  display.setCursor(69,16);
  display.print(boost,1);
  display.setTextSize(1);
  display.setCursor(25,50);
  display.print(F("!! WARNING !!"));
  }
}

void boostpressbar() {
  int barwidthmax = 104; //How many pixels wide the bar is going to be at its max reading
  float barpxperpsi = 0;
  float bardrawpx = 0;
  int i;
  barpxperpsi = barwidthmax / warnpsi; //Create how many pixels we need to draw up to our warning / max PSI
  bardrawpx = (barpxperpsi * boost) + 0.5; //Adding 0.5 to get the extra out of rounding errors and make it tidy (ish)
  
  if (bardrawpx >= barwidthmax){ //Make sure we never draw more than the maximum pixels!
    bardrawpx = barwidthmax;
  }
  
  display.fillRect(12,35, barwidthmax,10, BLACK); //Wipe the bar before drawing it again to allow us to make the bar move properly
  display.fillRect(12,35, bardrawpx,10, WHITE);
  for (i = 11; i <= 106;){ //Segment the bar.
    i = i + 5; // Makes the blocks 4 pixels wide by drawing a black line every 5th pixel. This divides nicely by the total bar width
    display.drawFastVLine(i,35, 10, BLACK); // Location, Height, Colour
  }
  
}
