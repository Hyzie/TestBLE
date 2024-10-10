#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "BLEConfig.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LEDWIFI 27  
#define LEDRF 33 
#define LED1 34 
#define LED2 35

#define WiFi_Mode 15
#define BTN 32

#define MOSI 23
#define MISO 19
#define CLK 18
#define CE 4
#define CSE 5

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET    -1 
#define SCREEN_ADDRESS 0x3C 

#define SDA_PIN 21
#define SCL_PIN 22 

#define TX1 1
#define RX1 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

IPAddress server_addr(103, 200, 23, 149);
char user[] = "optimaro_hoang_dev_esp32";
char password[] = "Hoang2468@";
char db[] = "optimaro_hoang_dev";

char* ssid = "";
char* pass = "";

char* device_id = "IOT_RF";

WiFiClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor* cursor;

/*------------------------------*/
// Procedure time:
unsigned long read_data_timer = 0;
unsigned long send_data_timer = 0;
unsigned long show_data_timer = 0;
unsigned long rs485_timer = 0;
/*------------------------------*/

volatile int state = 0;

