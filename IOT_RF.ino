#include "system_config.h"

unsigned long lastDebounce = 0;

void IRAM_ATTR handleButtonPress()
{
  unsigned long cur = millis();

  if (cur - lastDebounce > 100) {
    if (state > 3) {
      state = 0;
    }
    else
      state++;

    lastDebounce = cur;
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  Serial2.begin(9600, SERIAL_8N1, RX1, TX1);

  pinMode(LEDWIFI, OUTPUT);
  pinMode(LEDRF, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(WiFi_Mode, INPUT_PULLUP);
  pinMode(BTN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BTN), handleButtonPress, FALLING);

  Wire.begin(SDA_PIN, SCL_PIN);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(1000); 

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Setting up..."));
  display.display();
  delay(1000);

  ConnectToDB();
  // display.clearDisplay();
  // devide();
  // display.display();
  ShowRF();

}

void loop() {
  // if(millis() - read_data_timer > 50 || read_data_timer == 0)
  // {
  //   read_data_timer = millis();

  //   if(Serial2.available())
  //   {
  //     String data = Serial2.readString();
  //     Serial.println(data);
  //   }

  //   if(Serial2.available())
  //   {
  //     String data = Serial2.readString();
  //     Serial.println(data);
  //   }
  // }

  // if(millis() - send_data_timer > 1000 || send_data_timer == 0)
  // {
  //   send_data_timer = millis();

  //   char PROGMEM test_string[256];
  //   // sprintf(test_string, "*GD,%.3f,%.7f,%.7f,%d,%.3f,%.3f,%d#", utc, lat, lon, z, km, knots, day);

  //   Serial.println(test_string);

  //   if(conn.connected())
  //   {
  //     // SendData(temp, humid, a1, a2, d1, d2);
  //     // ParseStringAndSend(test_string);
  //   }
  //   else
  //   {
  //     conn.close();
  //     if(conn.connect(server_addr, 3306, user, password, db)) 
  //     {
  //       Serial.printf("Reconnected to database: %s", db);
  //     }
  //   }
  // }

  if(millis() - show_data_timer > 200 || show_data_timer == 0)
  {
    show_data_timer = millis();

    switch(state)
    {
      case 0:
      {
        ShowWiFiInfo(ssid);
        break;
      }
      
      case 1:
      {
        ShowRF();
        break;
      }
      
      case 2:
      {
        ShowUART1();
        break;
      }

      case 3: 
      {
        ShowUART2();
        break;
      }
    }
  }
}


void devide()
{
  display.drawLine(0, 0, 128, 0, WHITE);
  display.drawLine(0, 0, 0, 63, WHITE);
  display.drawLine(0, 63, 127, 63, WHITE);
  display.drawLine(127, 0, 127, 63, WHITE);
  
  display.drawLine(32, 16, 32, 64, WHITE);
  display.drawLine(0, 16, 128, 16, WHITE);
  display.drawLine(0, 32, 128, 32, WHITE);
  display.drawLine(0, 48, 128, 48, WHITE);
}

void ShowWiFiInfo(char* SSID)
{
  display.clearDisplay();

  display.drawLine(0, 0, 128, 0, WHITE);
  display.drawLine(0, 0, 0, 63, WHITE);
  display.drawLine(0, 63, 127, 63, WHITE);
  display.drawLine(127, 0, 127, 63, WHITE);

  display.setCursor(5, 10);
  display.print(F("WiFi: "));
  display.setCursor(5, 32);
  display.println(F(SSID));

  display.display();
}

void ShowRF(){
  display.clearDisplay();
  devide();

  display.setCursor(10, 5);
  display.print(F("RF Module"));

  display.display();
}

void ShowUART1(){
  display.clearDisplay();
  devide();

  display.setCursor(10, 5);
  display.print(F("UART1"));

  display.display();
}

void ShowUART2(){
  display.clearDisplay();
  devide();

  display.setCursor(10, 5);
  display.print(F("UART2"));

  display.display();
}

void ConnectToDB()
{
  Serial.println("Connecting to WiFi...");
  ssid = stoca(Essid());
  pass = stoca(Epass());
  WiFi.begin(ssid, pass);

  display.clearDisplay();
  display.setCursor(4, 10);
  char dpl[128];
  sprintf(dpl, "Connecting to %s ...", ssid); 
  display.println(dpl);
  display.display();

  int count = 20;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    count--;
    if(count == 0)
    {
      BLEsetup();
      while(1)
      {
        BLErepeat();

        display.clearDisplay();
        display.setCursor(20, 10); 
        display.println("Bluetooth");
        display.setCursor(40, 30);
        display.println("is on.");
        display.display();

        if(WiFi.status() == WL_CONNECTED)
          break;
      }
    }


  }
 
  display.clearDisplay();
  display.setCursor(4, 20);
  display.println("Connected.");
  display.display();
  delay(500);

  Serial.println("\nConnected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Attempting to connect to MySQL...");

  if(conn.connect(server_addr, 3306, user, password, db)) 
  {
    display.clearDisplay();
    display.setCursor(10, 30);
    display.println("Ready!!!");
    display.display();
    delay(500);
  } 
  else
  {
    while(1)
    {
      if(conn.connect(server_addr, 3306, user, password, db)) 
      {
        delay(500);
        Serial.printf("Reconnected to database: %s", db);
        break;
      }
    }
  }
}

// Convert String into char array
char* stoca(const String& str) {
  char* charArray = new char[str.length() + 1];
  str.toCharArray(charArray, str.length() + 1);
  return charArray;
}

void SendData(float temp, float humid, int a1, int a2, int d1, int d2)
{
  char PROGMEM query[256];
  sprintf(query, "UPDATE iot_monitor SET temperature = %.2f, humidity = %.2f, analog_1 = %d, analog_2 = %d, digital_1 = %d, digital_2 = %d WHERE iot_id = '%s'", temp, humid, a1, a2, d1, d2, device_id);


  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
}