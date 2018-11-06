/*
 * Open-Gauge v0.10
 * This is very much in development and right now is just testing layout and code to get it about right
 * Once this is done we will start using real values from a pressure sensor.
 */

float boost = 0;
float atmopsi = 0;
float warnpsi = 35; // 35PSI of boost is quite a lot! We probably want to warn if we are going over this
int uishown = 0;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

Adafruit_BMP280 bme; // I2C


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.print(F("Open-Gauge"));
  display.println(F("v0.10"));
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
}

void loop() {
  
  // Set up some sensors
  atmopsi = ((float)bme.readPressure()*0.0001450377); // Create atmospheric pressure PSI for later. We will need it to 0 the boost pressure at altitude etc. Hence BMP250
  boost = ((float)bme.readPressure()*0.0001450377); // Use barometric pressure to PSI to create fake but live PSI for layout testing
  
  showui();
  interiortemp();
  voltmeter();
  boostgauge();
 
  // This sets up the boost gauge pressure "bar" display
  display.fillRect(12,35, 5,10, WHITE); // Location, Size, Colour
  display.fillRect(20,35, 5,10, WHITE);
  display.fillRect(28,35, 5,10, WHITE);
  display.fillRect(36,35, 5,10, WHITE);
  display.fillRect(44,35, 5,10, WHITE);
  display.fillRect(52,35, 5,10, WHITE);
  display.fillRect(60,35, 5,10, WHITE);
  display.fillRect(68,35, 5,10, WHITE);
  display.fillRect(76,35, 5,10, WHITE);
  display.fillRect(84,35, 5,10, WHITE);
  display.fillRect(92,35, 5,10, WHITE);
  display.fillRect(100,35, 5,10, WHITE);
  display.fillRect(108,35, 5,10, WHITE);
  
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
  uishown = 1;
  display.drawRect(0,0, 128, 64, WHITE); // Location, Size, Colour
  display.drawFastHLine(0,10, 128, WHITE); // Location, Length, Colour
  display.drawFastVLine(64,0, 10, WHITE); // Location, Height, Colour
  }
}

void interiortemp() {
  // Read temp a few times to "smooth" the values a bit.
  int i;
  float tempavg = 0;

  for (i = 0; i < 100; i++){
    tempavg = tempavg + bme.readTemperature();
  }
  tempavg = tempavg / 100;
  
  display.fillRect(2,2, 62,7, BLACK); // Testing drawing over only areas we have to rather than blanking the whole screen every time
  display.setTextSize(1);
  display.setCursor(12,2);
  display.print(tempavg,1); //Display to 1 decimal place
  display.print((char)247); //Use a proper degrees symbol
  display.print(F("C"));
}

void voltmeter() {
  display.fillRect(65,2, 62,7, BLACK); // Testing drawing over only areas we have to rather than blanking the whole screen every time
  display.setCursor(80,2);
  display.print(bme.readAltitude(1013.25),0); // Replace altimeter with voltage readout at a later date. Altimeter is just for testing UI layout
  display.setCursor(120,2);
  display.print(F("m"));
}

void boostgauge() {
  display.setCursor(12,16);
  display.setTextSize(2);
  display.print(F("PSI")); // Example display layout untill pressure sensor is wired in. Probably want to account for negative pressure at some point incase it's used on a petrol
  display.setCursor(67,16);
  display.print(boost,1);
}
