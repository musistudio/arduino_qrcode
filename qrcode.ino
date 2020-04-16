#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include "qrcode.h"
#define LCD_CS A3    // Chip Select goes to Analog 3
#define LCD_CD A2    // Command/Data goes to Analog 2
#define LCD_WR A1    // LCD Write goes to Analog 1
#define LCD_RD A0    // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// 定义常用颜色的十六进制
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;



// 保存上次串口的值
float last_value=-1.0;
String deviceId;
String timestamp;
String qrcode_data;




// 绘制二维码方法
void showQrcode(const char *text)
{
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, text);
  const int ps = 8; // 放大倍数
  const int top = 80;
  // 绘制二维码
  for (uint8_t y = 0; y < qrcode.size; y++)
  {
    for (uint8_t x = 0; x < qrcode.size; x++)
    {
      if (qrcode_getModule(&qrcode, x, y))
      {
        for (int xi = x * ps + 2; xi < x * ps + ps + 2; xi++)
        {
          for (int yi = y * ps + 2; yi < y * ps + ps + 2; yi++)
          {
            tft.drawPixel(xi, yi + top, BLACK);
          }
        }
      }
    }
  }
}

// 显示设备ID和时间戳方法
void showText(String deviceId, String timestamp, String value)
{
  tft.setCursor(0, 0);
  tft.setTextSize(2);                    // 设置字体大小
  tft.println();                         // 换行
  tft.println("Device ID: " + deviceId);  // 显示设备ID
  tft.println("time: " + timestamp); // 显示时间戳
  tft.println("value: " + value); // 显示时间戳
  tft.println();                         // 换行
}

void setup(void)
{
  Serial.begin(9600);

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.4\" TFT Breakout Board Pinout"));
#endif

  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325)
  {
    Serial.println(F("Found ILI9325 LCD driver"));
  }
  else if (identifier == 0x9328)
  {
    Serial.println(F("Found ILI9328 LCD driver"));
  }
  else if (identifier == 0x4535)
  {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }
  else if (identifier == 0x7575)
  {
    Serial.println(F("Found HX8347G LCD driver"));
  }
  else if (identifier == 0x9341)
  {
    Serial.println(F("Found ILI9341 LCD driver"));
  }
  else if (identifier == 0x7783)
  {
    Serial.println(F("Found ST7781 LCD driver"));
  }
  else if (identifier == 0x8230)
  {
    Serial.println(F("Found UC8230 LCD driver"));
  }
  else if (identifier == 0x8357)
  {
    Serial.println(F("Found HX8357D LCD driver"));
  }
  else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  }
  else if (identifier == 0x7793)
  {
    Serial.println(F("Found ST7793 LCD driver"));
  }
  else if (identifier == 0xB509)
  {
    Serial.println(F("Found R61509 LCD driver"));
  }
  else
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0xB509;
  }

  tft.begin(identifier);
}

void loop(void)
{
  tft.fillScreen(WHITE);
  unsigned long start = micros();
  tft.setTextColor(BLACK);
  

  String cmdData;
  int commaPosition;
  int index=0;
  String datas[3];
  while (Serial.available() > 0)
  {
    cmdData += char(Serial.read());
    delay(2);
  }
  String temp = cmdData;
  if (cmdData.length() > 0)
  {
    Serial.println(cmdData);
    do
    {
      commaPosition = cmdData.indexOf(','); //检测字符串中的逗号
      if (commaPosition != -1) //如果有逗号存在就向下执行
      {
        datas[index] = cmdData.substring(0, commaPosition);
        index = index+1;
        cmdData = cmdData.substring(commaPosition + 1, cmdData.length()); //打印字符串，从当前位置+1开始
      }
      else
      {
        if (cmdData.length() > 0)
          datas[index] = cmdData;
      }
    } while (commaPosition >= 0);
    if(datas[2].toFloat() >= last_value)
    {
      last_value = datas[2].toFloat();
      deviceId = datas[0];
      timestamp = datas[1];
      qrcode_data = temp;
    }
    else
    {
      showQrcode(&String(qrcode_data)[0]);
      showText(deviceId, timestamp, String(last_value));
      last_value = -1.0;
      delay(1000 * 300);
    }
    
  }


  
}
