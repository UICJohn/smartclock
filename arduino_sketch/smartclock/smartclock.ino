#include <Process.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Bridge.h>
#include <FileIO.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Mailbox.h>
// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_RST 8
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_MISO 12
#define TFT_CLK 13
#define BUFFPIXEL 20
#define PATH_PREFIX "/mnt/sda1/images/medium/"

Process dateTime;
int hours, minutes;
//, seconds;
//int lastSecond = -1; 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
String date;
void setup() {
  Serial.begin(9600);
  Bridge.begin();
  Mailbox.begin();
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(10,10);
  while(!FileSystem.begin()){
  //    TO DO SYSTEM STARTING
    printInfo("Intializing");
    printInfo("System Starting...");
  }
  
  runIfNotRunning();
}

void loop() {
  dateTimeCheck();
  printTime(hours,minutes, 70, 60);
  printDate(50,90);
  String mesg;
  while(Mailbox.messageAvailable()){
    Mailbox.readMessage(mesg);
    processMsg(mesg);
  }
}


void processMsg(String mesg){
  int col_index = mesg.indexOf(":");
  String key = mesg.substring(0, col_index);
  String val = mesg.substring(col_index+1, mesg.length());
  if(key == "temperature"){
    int f = val.indexOf(",");
    int s = val.lastIndexOf(",");
    String temp = val.substring(0, f);
    String high = val.substring(f+1, s);
    String low = val.substring(s+1, val.length());
    printWeatherInfo(temp, high, low, 90, 150);
  }else if(key == "temp_image"){
    char filename[40];
    val.toCharArray(filename, 40);
    bmpDraw(&filename[0], 10, 120);
  }else if(key == "humidity"){
    bmpDraw("/mnt/sda1/images/medium/humidity.bmp", 30, 280);
    printHumidity(val, 60, 300);
  }
}

void printHumidity(String humidity, int x, int y){
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(x, y);
  tft.setTextSize(1);
  tft.print(humidity);
  tft.println("%");
}
void runIfNotRunning(){
  if(!dateTime.running()){
    dateTime.begin("date");
    dateTime.addParameter("+%a. %d %b  %Y - %H:%M:%S");
    dateTime.run();
  }
}

void printInfo(String info){
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println(info);
}

void printDate(int x, int y){
  tft.setFont(&FreeSerif9pt7b);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(x, y);
  tft.setTextSize(1);
  tft.println(date);
}

void printTime(int h, int m, int x, int y){
  tft.setFont(&FreeMono18pt7b);
  tft.setCursor(x,y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  if(hours <= 9){
    tft.print(0);  
  }
  tft.print(h);
  tft.print(":");
  if (minutes <= 9) {
    tft.print(0);  // adjust for 0-9
  }
  tft.println(m);
}

void printWeatherInfo(String temp, String high, String low, int x, int y){
  tft.setFont(&FreeMono18pt7b);
  tft.setCursor(x,y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(temp);
  tft.print("'C");
  tft.setFont(&FreeSerif9pt7b);
  tft.setCursor(x+20, y+30);
  tft.print(low);
  tft.print(" 'C ~ ");
  tft.print(high);
  tft.print(" 'C");
}

void dateTimeCheck(){
  runIfNotRunning();
  while(dateTime.available() > 0) {
    String dateTimeString = dateTime.readString();
    String timeString = dateTimeString.substring(dateTimeString.indexOf("-")+1, dateTimeString.length());
    String dateString = dateTimeString.substring(0, dateTimeString.indexOf("-"));
    int firstColon = timeString.indexOf(":");
    int secondColon = timeString.lastIndexOf(":");
    String hourString = timeString.substring(0, firstColon);
    String minString = timeString.substring(firstColon + 1, secondColon);
//    String secString = timeString.substring(secondColon + 1);
    if(hourString.toInt() != hours){
      tft.fillRect(70, 35, 40, 30, ILI9341_BLACK);
    }
    hours = hourString.toInt();
    if(minString.toInt() != minutes){
      tft.fillRect(135, 35, 40, 30, ILI9341_BLACK);
    }
    minutes = minString.toInt();
    dateString.toUpperCase();
    if(date != dateString){
      tft.fillRect(50, 75, 150, 20, ILI9341_BLACK);
    }
    date = dateString;
    date.toUpperCase();
  }
}

void bmpDraw(char *filename, uint8_t x, uint16_t y) {
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  tft.setCursor(x, y);
  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  File bmpFile = FileSystem.open(filename);
  if (bmpFile == NULL) {
    Serial.println("File not found");
    return;
  }
  else {
    Serial.println("File found");
  }

  // Parse BMP header
  if(read16(&bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(&bmpFile));
    (void)read32(&bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(&bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(&bmpFile));
    bmpWidth  = read32(&bmpFile);
    bmpHeight = read32(&bmpFile);
    if(read16(&bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(&bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(&bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

uint16_t read16(File *f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f->read(); // LSB
  ((uint8_t *)&result)[1] = f->read(); // MSB
  return result;
}

uint32_t read32(File *f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f->read(); // LSB
  ((uint8_t *)&result)[1] = f->read();
  ((uint8_t *)&result)[2] = f->read();
  ((uint8_t *)&result)[3] = f->read(); // MSB
  return result;
}
