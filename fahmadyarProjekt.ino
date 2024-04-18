#include <Wire.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "includedImages.c"
#include "customImages.c"	
#include <Fonts/FreeSans9pt7b.h>		
#include <Fonts/FreeSans18pt7b.h>		
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>	
#include "DHT.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Pins for the e-paper display
#define EPD_SCK  18
#define EPD_MOSI 23
#define EPD_CS   13
#define EPD_RST  21
#define EPD_DC   22
#define EPD_BUSY 14

#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 27

unsigned long startMillis;  // Speichern Sie die Startzeit
unsigned long currentMillis;  // Aktuelle Zeit
const unsigned long period = 15000;  // Die Zeitperiode in Millisekunden (10 Sekunden in diesem Fall)

int state = 0;

DHT dht(DHTPIN, DHTTYPE, INPUT_PULLUP);

// Create an instance of the display (using the right constructor for your display type)
GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT> display(GxEPD2_750_T7(/*CS=*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RST, /*BUSY=*/ EPD_BUSY));

// Create an instance of the MPU6050 sensor
Adafruit_MPU6050 mpu;

void startScreen() {
  display.init();
  display.setFont(&FreeSans24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(2);
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(255, 240);
  display.println("Startbildschirm");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(315, 260);
  display.println("ESP32 E-Bilderrahmen");
  display.nextPage();
}

void rotate() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if(a.acceleration.x < 1 && a.acceleration.x > 0) {
    display.init();
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(2);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 20);
    display.println("Querformat");
    display.nextPage();
  } else if(a.acceleration.x < 11 && a.acceleration.x > 10) {
    display.init();
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(3);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 20);
    display.println("Hochformat");
    display.nextPage();
  } else if(a.acceleration.x <= -8 && a.acceleration.x >= -10) {
    display.init();
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(1);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0,20);
    display.println("Hochformat mit Ständer");
    display.nextPage();
  }
}

void roomData() {
  Serial.begin(9600);
  pinMode(MQ135_PIN, INPUT);  // Set the MQ135_PIN as INPUT
  dht.begin();
  display.init();
  display.setFont(&FreeSans24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(2);
  display.setFullWindow();

  // Full update to draw all the static elements
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 40);
    display.println("Umgebungsdaten");
    display.setFont(&FreeSansBold18pt7b);
    display.drawBitmap(0, 62, gImage_temp, 40, 40, GxEPD_BLACK);
    display.setCursor(50, 94);
    display.println("Temperatur:");
    display.drawBitmap(0, 114, gImage_humiditiy, 40, 40, GxEPD_BLACK);
    display.setCursor(50, 146);
    display.println("Luftfeuchtigkeit:");
    display.drawBitmap(0, 166, gImage_heat_index, 40, 40, GxEPD_BLACK);
    display.setCursor(50, 198);
    display.println("Hitze-Index:");
    display.drawBitmap(0, 218, gImage_co2, 40, 40, GxEPD_BLACK);
    display.setCursor(50, 250);
    display.println("Kohlenstoffdioxid:");
  } while (display.nextPage());

  while (true) {  // Infinite loop to keep updating the data
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float hic = dht.computeHeatIndex(t, h, false);
    pinMode(MQ135_PIN, INPUT); // Set the MQ135_PIN as an input
    int mq135_value = analogRead(MQ135_PIN);  // Read the value from the sensor

    // Partial update to refresh the sensor values
    display.setPartialWindow(665, 70, 200, 200);  // Set the partial window to cover the area of the sensor values
    display.firstPage();
    do {
      display.setFont(&FreeSans18pt7b);
      display.setCursor(680, 94);
      display.println(t);
      display.setCursor(773, 94);
      display.println("C");
      display.setCursor(680, 146);
      display.println(h);
      display.setCursor(770, 146);
      display.println("%");
      display.setCursor(680, 198);
      display.println(hic);
      display.setCursor(773, 198);
      display.println("C");
      display.setCursor(665, 250);
      display.println(mq135_value);
      display.setCursor(730, 250);
      display.println("ppm");
    } while (display.nextPage());
  }

    delay(1000);  // Wait for 1 seconds before the next update
}

void slideShow() {
  display.init();
  display.setRotation(2);
  display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, gImage_grimes, 800, 480, GxEPD_BLACK);
  delay(3750);
  display.nextPage();
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, gImage_angelie, 800, 480, GxEPD_BLACK);
  delay(3750);
  display.nextPage();
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, gImage_flo, 800, 480, GxEPD_BLACK);
  delay(3750);
  display.nextPage();
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, gImage_art, 800, 480, GxEPD_BLACK);
  delay(3750);
  display.nextPage();
  
}

void setup() {
  /*
  * MPU-6050
  */
  Serial.begin(9600);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);

  Wire.begin();
  
  startMillis = millis();
}

void loop() {
  currentMillis = millis();
  if (currentMillis - startMillis >= period) {
    state++;
    startMillis = currentMillis; // reset the start time
  }

  if (state % 4 == 0) {
    startScreen();
  } else if (state % 4 == 1) {
    rotate();
  } else if (state % 4 == 2) {
    slideShow();
  } else if (state % 4 == 3) {
    roomData();
  } 
}