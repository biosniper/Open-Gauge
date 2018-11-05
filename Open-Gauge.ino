/*
 * Open-Gauge v0.06
 * This is very much in development and right now is just testing layout and code to get it about right
 * Once this is done we will start using real values from a pressure sensor
 */

int mapsen = 0;
float boost = 0;
float mapsenval = 0;
float warnpsi = 20.5;
float baropress = 0;

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
  display.println(F("v0.06"));
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
  boost = ((float)bme.readPressure()*0.0001450377); // Use barometric pressure to PSI to create fake but live PSI for layout testing
  display.drawRect(0,0, 128, 64, WHITE); // Location, Size, Colour
  display.drawFastHLine(0,10, 128, WHITE); // Location, Length, Colour
  display.drawFastVLine(64,0, 10, WHITE);
  display.setTextSize(1);
  display.setCursor(12,2);
  display.print(bme.readTemperature(),1);
  display.print(F("*C"));
  display.setCursor(72,2);
  display.print(bme.readAltitude(1013.25),0); // Maybe replace altimiter with voltage readout because how useful is an altimiter anyway?
  display.setCursor(120,2);
  display.print(F("m"));
  display.setCursor(12,16);
  display.setTextSize(2);
  display.print(F("PSI")); // Example display layout untill pressure sensor is wired in
  display.setCursor(67,16);
  display.print(boost,1);
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
  display.display();
  display.clearDisplay();

}
